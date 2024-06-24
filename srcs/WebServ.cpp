//Webserv
#include "../includes/WebServ.hpp"
#include "../includes/Connection.hpp"
#include <cstdio>
#include <dirent.h>

void WebServ::run(Connection * clt, char * envp[])
{
	if (clt->getReadStatus() < COMPLETE)
	{
		return ;
	}

	switch(clt->getMethod())
	{
		case GET:
			runGET(clt, envp);
		case POST:
			runPOST(clt, envp);
		case DELETE:
			runDELETE(clt, envp);
	}
	clt->changeStatus(COMPLETE);
}

void WebServ::runGET(Connection * clt, char * envp[])
{
	eProcessType procType = clt->getProcType();
	if (procType == FILE)
	{
		clt->fillRequest();
	}
	else if (procType == AUTO_INDEX)
	{
		DIR * dir = opendir(clt->getAbsolutePath());
		if (dir == NULL)
		{
			clt->fillRequest();
		}
		else
		{
			std::vector<std::string> fileList;
			getFileList(fileList, dir);
			clt->fillRequest(fileList);
			closedir(dir);
		}
	}
	else if (procType == CGI)
	{
		clt->processCGI(this->mKqueue, envp);
	}
}

void WebServ::runPOST(Connection * clt, char * envp[])
{
	eProcessType procType = clt->getProcType();
	if (procType == FILE)
	{
		DIR * dir = opendir(clt->getAbsolutePath());
		if (dir == NULL)
		{
			throw std::exception(); // 404
		}
		closedir(dir);

		std::string fileName = clt->getAbsolutePath();
		fileName += ".";
		fileName += clt->getContentType();
		std::ofstream file;
		file.open(fileName.c_str());
		if (!file.is_open())
		{
			throw std::exception(); // 500
		}
		file << clt->getReqBody();
		file.close();
	}
	else if (procType == CGI)
	{
		clt->processCGI(this->mKqueue, envp);
	}
}

void WebServ::runDELETE(Connection * clt, char * envp[])
{
	eProcessType procType = clt->getProcType();
	if (procType == FILE)
	{
		clt->removeFile();
	}
	else if (procType == AUTO_INDEX)
	{
		DIR * dir = opendir(clt->getAbsolutePath());
		if (dir == NULL)
		{
			clt->removeFile();
		}
		else
		{
			struct dirent * file = readdir(dir); // 첫 . 는 버린다.
			file = readdir(dir); // 두번째 .. 도 버린다.
			while ((file = readdir(dir)) != NULL)
			{
				std::string str = file->d_name;
				clt->removeFile(str);
			}
			closedir(dir);
		}
	}
	else if (procType == CGI)
	{
		clt->processCGI(this->mKqueue, envp);
	}
}

void WebServ::getFileList(std::vector<std::string> & list, DIR * dir)
{
	struct dirent * file = readdir(dir); // 첫 . 는 버린다.
	while ((file = readdir(dir)) != NULL)
	{
		std::string str = file->d_name;
		list.push_back(str);
	}
}
