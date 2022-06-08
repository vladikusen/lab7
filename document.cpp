#include "document.h"


Document::Document() : root(nullptr), result("")
{
}

Document::Document(const Document& that)
	: result(that.result), rootWindowLength(that.rootWindowLength)
{
	root = std::make_shared<Block>(rootWindowLength);
	that.root->copy(root);
	root = root->getInner();
}

Document::Document(std::istream& myFile)
{
	std::string text;
	std::string line;
	while (std::getline(myFile, line)) {
		text += line + '\n';
	}
	load(text);
}



Document::Document(const std::string& text)
{
	load(text);
}

Document& Document::operator=(const Document& that)
{
	initialText = that.initialText;
	result = that.result;
	root = std::make_shared<Block>(that.rootWindowLength);
	that.root->copy(root);
	root = root->getInner();
	rootWindowLength = that.rootWindowLength;
	elementsCounter = that.elementsCounter;

	return *this;
}

Document& Document::operator=(Document&& that) noexcept
{
	result = std::move(that.result);
	root = std::move(that.root);
	elementsCounter = std::move(that.elementsCounter);
	rootWindowLength = std::move(that.rootWindowLength);

	return *this;
}


bool Document::isEmpty() const
{
	if (root == nullptr) {
		return false;
	}
	return true;
}

void Document::remove(size_t index)
{
	//root->getByIndex(index)->print();
	root->getByIndex(index)->removeElement();
	size_t counter = 0;
	root->setIndex(counter);
	elementsCounter = counter;
	result.clear();
	root->render(result);



}

void Document::update(size_t index, std::istream& myFile)
{
	auto forUpdateParent = root->getByIndex(index)->getParent();
	auto forUpdate = root->getByIndex(index);
	
	std::string text;
	std::string line;
	while (std::getline(myFile, line)) {
		text += line + '\n';
	}

	spE block = std::make_shared<Block>(forUpdateParent->getCleanWidth());
	block = block->createElement(text, forUpdateParent->getCleanWidth());
	forUpdateParent->insertUpdated(forUpdate, block);
	//forUpdate->removeElement();
	size_t counter = 0;
	root->setIndex(counter);
	elementsCounter = counter;
	result.clear();
	root->render(result);

}

void Document::update(size_t index, const std::string& text)
{

	auto forUpdateParent = root->getByIndex(index)->getParent();
	auto forUpdate = root->getByIndex(index);

	spE block = std::make_shared<Block>(forUpdateParent->getCleanWidth());
	block = block->createElement(text, forUpdateParent->getCleanWidth());
	forUpdateParent->insertUpdated(forUpdate, block);
	forUpdate->removeElement();
	size_t counter = 0;
	root->setIndex(counter);
	elementsCounter = counter;
	result.clear();
	root->render(result);
	
}

void Document::clear()
{
	result.clear();
	root = nullptr;
}

void Document::load(std::istream& myFile)
{
	std::string text;
	std::string line;
	while (std::getline(myFile, line)) {
		text += line + '\n';
	}

	spE block = std::make_shared<Block>(rootWindowLength);
	root = block->createElement(text, rootWindowLength);
	size_t counter = 0;
	root->setIndex(counter);
	elementsCounter = counter;
	root->render(result);
}

void Document::load(const std::string& _text)
{
	spE block = std::make_shared<Block>(rootWindowLength);
	root = block->createElement(_text, rootWindowLength);
	size_t counter = 0;
	root->setIndex(counter);
	elementsCounter = counter;
	root->render(result);
}

std::string Document::render() const
{
	return result;
}

