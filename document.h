#ifndef __DOCUMENT_H__
#define __DOCUMENT_H__

#include <string>	
#include <iostream>
#include "element.h"
#include <math.h>

class Document
{
private:
	spE root;
	size_t rootWindowLength{100};
	std::string result;
	std::string initialText;
	size_t elementsCounter;
public:
	Document();
	Document(const Document&);
	explicit Document(std::istream& stream);
	explicit Document(const std::string& text);

	Document& operator=(const Document&);
	Document& operator=(Document&&) noexcept;
	
	bool isEmpty() const;
	size_t count() const { return elementsCounter; };
	void remove(size_t index);
	void update(size_t index, std::istream& stream);
	void update(size_t index, const std::string& text);
	void clear();

	void load(std::istream& stream);
	void load(const std::string& text);

	std::string render() const;
};


#endif

