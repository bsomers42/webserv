#ifndef LOCATION_HPP
# define LOCATION_HPP
# include <string>
# include <vector>
# include <map>
# include <utility>

class Location
{
	private:

		std::string					_locationMatch;
		std::string					_locationModifier;
		std::vector<std::pair<std::string, std::string> >	_locationDirectives;
		std::map<int, std::string>	_errorPages;

		std::string					_root;
		std::vector<std::string>	_indexes;
		bool						_autoindex;
		unsigned long long			_maxBodySize;
		std::vector<std::string>	_allowed;
		std::vector<std::string>	_denied;
		std::pair<int, std::string>	_return;
		std::string					_uploadDir;
	
	public:
		// functions
		void                setModifier(std::string modifier);
		const std::string   getModifier(void) const;

		void                setMatch(std::string match);
		const std::string   getMatch(void) const;

		void	            setRoot(std::string root);
		std::string	        getRoot() const;
		
		void	                    setIndexes(std::vector<std::string> indexes);
		std::vector<std::string>	getIndexes() const;
		std::string                 getIndex(size_t i) const;

		void				setAutoindex(bool autoindex);
		bool				getAutoindex() const;
		
		void				setMaxBodySize(unsigned int maxBodySize);
		unsigned int		getMaxBodySize() const;
		
		void				setErrorPages(std::map<int, std::string> errorPages);
		std::map<int, std::string> const &	getErrorPages() const;

		void	                    setAllowed(std::vector<std::string> allowed);
		std::vector<std::string>	getAllowed() const;
		std::string                 getAllow(size_t i) const;

		void	                    setDenied(std::vector<std::string> denied);
		std::vector<std::string>	getDenied() const;
		std::string                 getDeny(size_t i) const;

		void                				setReturn(int code, std::string text);
		const std::pair<int, std::string>   getReturn(void) const;

		void	            setUploadDir(std::string uploadDir);
		std::string	        getUploadDir() const;
};

#endif