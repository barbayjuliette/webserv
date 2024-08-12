/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 13:15:27 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/12 14:28:10 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

/*
** -------------------------- INIT STATIC VARIABLES ---------------------------
*/

std::map<int, std::string>	Response::_status_lookup;

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Response::Response() {}

Response::Response(Request &request, ServerConfig *conf) : _body(""), _config(conf)
{
	if (this->_status_lookup.empty())
		init_status_lookup();

	_location = _config->matchLocation(request.getPath());
	if (_location->getRedirect().size() > 0)
	{
		_path = _location->getRoot() + _location->getRedirect();
		std::cout << CYAN << "RESPONSE - REDIRECT: " << RESET << _path << '\n';
		this->_status_code = 301;
		this->_status_text = "Moved Permanently";
		_headers["Location"] = "/database/";
		setHeaders(request);
		// std::cout << "FULL REDIRECT RESPONSE: \n";
		// std::cout << getFullResponse() << std::endl;
		return ;
	}
	else
	{
		_path = _location->getRoot() + request.getPath().substr(1, std::string::npos);
		std::cout << CYAN << "RESPONSE - PATH: " << RESET << _path << '\n';
	}

	setContentType(_path);

	if (request.getError() != NO_ERR && request.getError() != NOT_SUPPORTED)
	{
		set_error(400);
		_headers["Content-Length"] = intToString(this->_body.size());
		std::cout << "Error: " << request.getError() << std::endl;
	}
	else if (!method_is_allowed(request.getMethod(), _location->getAllowedMethods()))
		set_allow_methods(false);
	else if (request.getMethod() == "GET")
		respond_get_request(request);
	else if (request.getMethod() == "POST")
		respond_post_request(request);
	else if (request.getMethod() == "DELETE")
		respond_delete_request();
	else
		set_allow_methods(false);

	setHeaders(request);
}

void	Response::setHeaders(Request &request)
{
	_headers["Cache-Control"] = "no-cache, private";
	this->_http_version = request.getHttpVersion();

	if (this->_http_version == "HTTP/1.1" && request.getHeaders()["connection"] != "close")
		_headers["Connection"] = "keep-alive";
	else
		_headers["Connection"] = "close";

	getDate();
	setFullResponse();
}

Response::Response( const Response & src ) :
_status_code(src._status_code),
_status_text(src._status_text),
_http_version(src._http_version),
_body(src._body),
_full_response(src._full_response),
_path(src._path),
_headers(src._headers),
_config(src._config),
_location(src._location)
{

}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Response::~Response() {}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Response &				Response::operator=( Response const & rhs )
{
	if (this != &rhs)
	{
		this->_status_code = rhs._status_code;
		this->_status_text = rhs._status_text;
		this->_http_version = rhs._http_version;
		this->_body = rhs._body;
		this->_full_response = rhs._full_response;
		this->_path = rhs._path;
		this->_headers = rhs._headers;
		this->_config = rhs._config;
		this->_location = rhs._location;
	}
	return (*this);
}

/*
** --------------------------------- METHODS ----------------------------------
*/

bool	Response::method_is_allowed(std::string method, std::vector<std::string> allowed)
{
	std::vector<std::string>::iterator it = std::find(allowed.begin(), allowed.end(), method);
	if (it != allowed.end())
	{
		return (true);
	}
	return (false);
}

std::string	Response::create_html(std::string source, std::string req_path)
{
	if (req_path[req_path.size() - 1] != '/')
		req_path += '/';

	std::string	link = req_path + source;
	std::string	html = "<li><a href=\"" + link + "\">" + source + "</a></li>";
	return (html);
}

void	Response::create_directory_listing(std::string path, std::string req_path)
{
	std::string		no_slash = path.substr(0, path.size() - 1);
	DIR* dir = opendir(no_slash.c_str());
	if (!dir)
	{
		set_error(404);
		_headers["Content-Length"] = intToString(this->_body.size());
		return ;
	}
	
	struct dirent*			dr;
	std::stringstream		index;
	index << "<h1>Index</h1><ul>";

	while ((dr = readdir(dir)))
	{
		std::string	name = dr->d_name;
		if (name == "." || name == ".." || name[0] == '.')
			continue;
		index << create_html(dr->d_name, req_path);
	}
	index << "</ul>";
	closedir(dir);

	_body = index.str();
	_headers["Content-Length"] = intToString(this->_body.size());
	set_success();
}

int		Response::is_directory(std::string req_path)
{
	std::string					dir_path = this->_path;

	struct	stat				filename;
	if (stat(dir_path.c_str(), &filename) == -1 || !S_ISDIR(filename.st_mode))
		return (1); // Does not exist, or is not a directory => No need to check index or autoindex

	// Is directory: Set path to index file
	if (dir_path[dir_path.size() - 1] != '/') // Append backslash if not present
		dir_path += "/";
	setPath(dir_path + _location->getIndex());

	if (access(this->_path.c_str(), F_OK) == 0) // Index exists, we show that one
	{
		setContentType(_path);
		return (1); // Continue the respond_get_request
	}
	else if (_location->getAutoindex() == true) // If no index, check if autoindex is enabled
		create_directory_listing(dir_path, req_path);
	else // If no index and autoindex is disabled
		set_error(403);

	_headers["Content-Length"] = intToString(this->_body.size());
	setContentType(_path);
	return (0);
}

void	Response::respond_get_request(const Request &request)
{
	std::string	req_ext = extract_cgi_extension(request.getPath());

	if (_location->getCGIExec(req_ext).size() > 0)
	{
		CGIHandler*	cgi = new CGIGet(request, _location, req_ext);
		process_cgi_response(cgi);
		delete (cgi);
		return ;
	}

	if (is_directory(request.getPath()) == 0)
		return ;

	char						c;
	std::ifstream				page(this->_path.c_str());
	if (page.good())
	{
		while (page.get(c))
			_body += c;
		set_success();
	}
	else
		set_error(404);
	_headers["Content-Length"] = intToString(this->_body.size());
	page.close();
}

void	Response::respond_post_request(const Request &request)
{
	std::ifstream				page(this->_path.c_str());
	std::string	req_ext = extract_cgi_extension(request.getPath());

	if (_location->getCGIExec(req_ext).size() > 0) // Check if finishes with CGI extension.
	{
		CGIHandler*	cgi = new CGIPost(request, _location, req_ext);
		process_cgi_response(cgi);
		delete cgi;
	}
	else if (!page.good())// Path does not exist : 404
		set_error(404);
	else // Page exists but cgi is not enabled
		set_allow_methods(true);
	_headers["Content-Length"] = intToString(this->_body.size());
}

void	Response::respond_delete_request()
{
	if (access(this->_path.c_str(), F_OK) == -1)
		set_error(404);
	else if (remove(this->_path.c_str()) == 0)
	{
		set_success();
		_body = "<p>Resource deleted</p>";
	}
	else
	{
		if (errno == EACCES || errno == EPERM)
			set_error(403);
		else
			set_error(500);
		std::cerr << "Error deleting resource\n";
	}
	_headers["Content-Length"] = intToString(this->_body.size());
}

/*
** -------------------------------- CGI UTILS ---------------------------------
*/

std::string	Response::extract_cgi_extension(const std::string& req_path)
{
	size_t	i = req_path.rfind('.');
	if (i == std::string::npos)
		return ("");
	return (req_path.substr(i, std::string::npos));
}

void	Response::process_cgi_response(CGIHandler* cgi)
{
	if (cgi->getError() != 0)
		set_error(cgi->getError());
	else
	{
		_body = cgi->getHtml();
		_headers["Content-Type"] = cgi->getContentType();
		set_success();
	}
	_headers["Content-Length"] = intToString(this->_body.size());
}

/*
** ---------------------------------- STATUS ----------------------------------
*/

void	Response::init_status_lookup(void)
{
	this->_status_lookup[200] = "OK";
	this->_status_lookup[301] = "Moved Permanently";
	this->_status_lookup[400] = "Bad Request";
	this->_status_lookup[403] = "Forbidden";
	this->_status_lookup[404] = "Not Found";
	this->_status_lookup[405] = "Method Not Allowed";
	this->_status_lookup[408] = "Request Timeout";
	this->_status_lookup[500] = "Internal Server Error";
}

void	Response::set_success(void)
{
	this->_status_code = 200;
	this->_status_text = _status_lookup[_status_code];
}

void	Response::set_error(int code)
{
	this->_status_code = code;
	this->_status_text = _status_lookup[_status_code];
	this->_body = get_error_page(_status_code);
}

std::string		Response::get_error_page(int num)
{	
	std::string					error_path = _config->getErrorPage(num);
	
	if (error_path.empty())
		error_path = "./default_errors/" + intToString(num) + ".html";
	else
		error_path = "./" + error_path;

	std::ifstream				error_page(error_path.c_str());
	std::string					str;
	char						c;

	if (error_page.good())
	{
		while (error_page.get(c))
			str += c;
	}
	else
	{
		std::cout << "Error loading Error page " << num << std::endl;
		str = "<h1>Error" +  intToString(num) + "</h1><p>" + _status_text + "</p>";
	}
	error_page.close();
	_headers["Content-Type"] = "text/html";
	return (str);
}

std::string	Response::intToString(int num)
{
	std::stringstream	stream;
	stream << num;
	return (stream.str());
}

/*
** ---------------------------------- SETTER ----------------------------------
*/

void	Response::setPath(std::string new_path)
{
	this->_path = new_path;
}

void	Response::setFullResponse()
{
	std::stringstream								stream;
	std::map<std::string, std::string>::iterator	it;

	stream << this->_http_version << " " << this->_status_code << " " << this->_status_text << "\r\n";

	for (it = _headers.begin(); it != _headers.end(); ++it)
	{
		stream << it->first << ": " << it->second << "\r\n";
	}
	stream << "\r\n";
	if (!_body.empty())
		stream << this->_body;

	this->_full_response = stream.str();
}

void	Response::setContentType(std::string path)
{
	std::size_t		pos = path.find_last_of(".");
	std::string		ext;

	if (pos == std::string::npos)
	{
		_headers["Content-Type"] = "text/plain";
		return ;
	}
	ext = path.substr(pos + 1, path.size() - pos + 1);
	if (ext == "html" || ext == "htm")
		_headers["Content-Type"] = "text/html";
	else if (ext == "css")
		_headers["Content-Type"] = "text/css";
	else if (ext == "js")
		_headers["Content-Type"] = "text/javascript";
	else if (ext == "jpg" || ext == "jpeg")
		_headers["Content-Type"] = "image/jpeg";
	else if (ext == "png")
		_headers["Content-Type"] = "image/png";
	else if (ext == "gif")
		_headers["Content-Type"] = "image/gif";
	else if (ext == "pdf")
		_headers["Content-Type"] = "application/pdf";
	else
		_headers["Content-Type"] = "text/plain";
}

void	Response::set_allow_methods(bool post)
{
	std::vector<std::string>::iterator it;
	std::string							methods;
	std::vector<std::string> 			allowed_methods = _location->getAllowedMethods();

	for (it = allowed_methods.begin(); it != allowed_methods.end(); it++)
	{
		if (post && *it == "POST")
			continue ;
		methods += *it;
		if (it + 1 != allowed_methods.end())
		{
			if(!(*(it + 1) == "POST" && post))
				methods += ", ";
		}
	}
	_headers["Allow"] = methods;
	set_error(405);
	_headers["Content-Length"] = intToString(this->_body.size());
	std::cout << "ALLOWED METHODS: " << methods << std::endl;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

int	Response::getStatusCode() const
{
	return (this->_status_code);
}

std::string	Response::getStatusText() const
{
	return (this->_status_text);
}

std::string	Response::getHttpVersion() const
{
	return (this->_http_version);
}

std::map<std::string, std::string>	Response::getHeaders() const
{
	return (this->_headers);
}

std::string	Response::getBody() const
{
	return (this->_body);
}

std::string	Response::getPath() const
{
	return (this->_path);
}

std::string	Response::getFullResponse() const
{
	return (this->_full_response);
}

void		Response::getDate()
{
	time_t time;
	std::time(&time);

	struct tm *gmt;
	gmt = std::gmtime(&time);
	char formatted_date[30];
	std::strftime(formatted_date, sizeof(formatted_date), "%a, %d %b %Y %H:%M:%S GMT", gmt);
	_headers["Date"] = formatted_date;
}

/* ************************************************************************** */