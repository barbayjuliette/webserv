/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbarbay <jbarbay@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 13:15:27 by jbarbay           #+#    #+#             */
/*   Updated: 2024/08/02 20:38:04 by jbarbay          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Response::Response() {}

Response::Response(Request &request, ServerConfig *conf) : _config(conf)
{
	_location = _config->matchLocation(request.getPath());
	_path = _location->getRoot() + request.getPath().substr(1, std::string::npos);
	std::cout << "RESPONSE - PATH: " << _path << '\n';

	setContentType(_path);

	if (request.getError() != NO_ERR &&  request.getError() != NOT_SUPPORTED)
	{
		set_error(400, "Bad Request");
		std::cout << "Error: " << request.getError() << std::endl;
	}
	else if (!method_is_allowed(request.getMethod(), _location->getAllowedMethods()))
		this->set_allow_methods(false);
	else if (request.getMethod() == "GET")
		this->respond_get_request(request);
	else if (request.getMethod() == "POST")
		this->respond_post_request(request);

	else if (request.getMethod() == "DELETE")
		this->respond_delete_request();
	else
		this->set_allow_methods(false);

	_headers["Cache-Control"] = "no-cache, private";
	this->_http_version = request.getHttpVersion();

	if (this->_http_version == "HTTP/1.1" && request.getHeaders()["connection"] != "close")
		_headers["Connection"] = "keep-alive";
	else
		_headers["Connection"] = "close";

	getDate();
	setFullResponse();
	// std::cout << this->_path << std::endl;
	//  Content-Length if there is a body
	// /!\ Body can be empty string, would be valid request
}

Response::Response( const Response & src ) :
_status_code(src._status_code),
_status_text(src._status_text),
_http_version(src._http_version),
_body(src._body),
_full_response(src._full_response),
_headers(src._headers),
_config(src._config)
{

}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Response::~Response() {}

/*
** --------------------------------- METHODS ----------------------------------
*/

int		Response::method_is_allowed(std::string method, std::vector<std::string> allowed)
{
	std::vector<std::string>::iterator it = std::find(allowed.begin(), allowed.end(), method);
	if (it != allowed.end())
	{
		return (1);
	}
	return (0);
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
		set_error(404, "Not Found");
		_headers["Content-Length"] = intToString(this->_body.size());
		return ;
	}
	
	struct dirent*			dr;
	std::stringstream		index;
	index << "<h1>Index</h1><ul>";

	while ((dr = readdir(dir)))
	{
		std::string	name = dr->d_name;
		if (name == "." || name == "..")
			continue;
		index << create_html(dr->d_name, req_path);
	}
	index << "</ul>";
	closedir(dir);

	_body = index.str();
	_headers["Content-Length"] = intToString(this->_body.size());
	this->_status_code = 200;
	this->_status_text = "OK";
}

int		Response::is_directory(std::string req_path)
{
	bool						autoIndex = _location->getAutoindex();
	std::string					dir_path = _path;

	struct	stat				filename;
	stat(_path.c_str(), &filename);

	if (this->_path[this->_path.size() - 1] == '/')
	{
		setPath(this->_path + "index.html");
	}
	else if (S_ISDIR(filename.st_mode))
	{
		setPath(this->_path + "/index.html");
		dir_path += "/";
	}
	else
		return (1);

	if (access(this->_path.c_str(), F_OK) == 0) // Index exists, we show that one
	{
		setContentType(_path);
		return (1); // Continue the respond_get_request
	}
	else if (autoIndex == true)
		create_directory_listing(dir_path, req_path);
	else
		set_error(404, "Not Found");

	_headers["Content-Length"] = intToString(this->_body.size());
	setContentType(_path);
	return (0);
}

void	Response::respond_get_request(const Request &request)
{
	if (is_directory(request.getPath()) == 0)
		return ;
	
	std::string		ext = ".py";
	int				length = request.getPath().size();

	if (request.getPath().substr(length - ext.size(), length) == ext)
	{
		std::cout << RED << "CGI GET \n" << RESET;
		CGIGet*	cgi = new CGIGet(request);
		_body = cgi->getHtml();
		_headers["Content-Length"] = intToString(this->_body.size());
		_headers["Content-Type"] = cgi->getContentType();
		delete (cgi);
		return ;
	}

	char						c;
	std::ifstream				page(this->_path.c_str());
	if (page.good())
	{
		while (page.get(c))
			_body += c;
		this->_status_code = 200;
		this->_status_text = "OK";
	}
	else
		set_error(404, "Not Found");
	_headers["Content-Length"] = intToString(this->_body.size());
	page.close();
}

void	Response::cgi_post_form(const Request &request)
{
	// std::cout << RED << "This is CGI\n" << RESET;
	CGIHandler*	cgi = new CGIHandler(request);
	_body = cgi->getHtml();
	_headers["Content-Length"] = intToString(this->_body.size());
	_headers["Content-Type"] = cgi->getContentType();
	// _headers["Location"] = "./subscribe.html";
	// std::cout << RED << "CGI DONE\n" << RESET;
	delete (cgi);
}

void	Response::respond_post_request(const Request &request)
{
	std::ifstream				page(this->_path.c_str());

	// TO DO Check only if finishes with extension.
	if (request.getPath().substr(0, 8) == "/cgi-bin")
		cgi_post_form(request);
	else if (!page.good())// Path does not exist : 404
		set_error(404, "Not Found");
	else // Page exists but not allowed to do POST
		set_allow_methods(true);
	_headers["Content-Length"] = intToString(this->_body.size());
}

int	Response::check_permission(void)
{
	std::string		database = "./" + _config->getRoot() + "database";
	int				length = database.size();

	if (this->_path.substr(0, length) != database)
	{
		set_error(403, "Forbidden");
		std::cout << "in check_permission\n";
		_headers["Content-Length"] = intToString(this->_body.size());
		return (0);
	}
	return (1);
}

void	Response::set_error(int code, std::string text)
{
	this->_status_code = code;
	this->_status_text = text;
	_body = get_error_page(_status_code);
}

void	Response::respond_delete_request()
{
	// We can delete anything from root/database/
	if (!check_permission())
		return ;

	if (access(this->_path.c_str(), F_OK) == -1)
		set_error(404, "Not found");
	else if (remove(this->_path.c_str()) == 0)
	{
		this->_status_code = 200;
		this->_status_text = "OK";
		_body = "<p>Resource deleted</p>";
	}
	else
	{
		if (errno == EACCES || errno == EPERM)
		{
			set_error(403, "Forbidden");
			std::cout << "checking error number\n";
		}
		else
			set_error(500, "Internal Server Error");
		std::cout << "Error deleting resource\n";
	}
	_headers["Content-Length"] = intToString(this->_body.size());
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
	set_error(405, "Method Not Allowed");
	_headers["Content-Length"] = intToString(this->_body.size());
	std::cout << "ALLOWED METHODS: " << methods << std::endl;
}

void		Response::getDate()
{
    time_t time;
    std::time(&time);

    struct tm *gmt;
    gmt = std::gmtime(&time);
    char formatted_date[30];
    std::strftime(formatted_date, sizeof(formatted_date), "%a, %d %b %Y %H:%M:%S GMT", gmt);
    // std::cout << std::string(formatted_date) << std::endl;
	_headers["Date"] = formatted_date;
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Response &				Response::operator=( Response const & rhs )
{
	if (this != &rhs)
	{
		this->_status_code = rhs._status_code;
	}
	return (*this);
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

void	Response::setPath(std::string new_path)
{
	this->_path = new_path;
}

std::string	Response::getFullResponse() const
{
	return (this->_full_response);
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
	// std::cout << "Extension: " << ext << std::endl;
	// std::cout << "Path: " << path << std::endl;
	// std::cout << "Content-type: " << _headers["Content-Type"] << std::endl;
}

/* ************************************************************************** */