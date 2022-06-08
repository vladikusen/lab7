#ifndef __ELEMENT_H__
#define __ELEMENT_H__

#include <memory>
#include <string>
#include <iostream>
#include <array>
#include <list>

class Element;

using spE = std::shared_ptr<Element>;

class Element : public std::enable_shared_from_this<Element>
{
protected:
	spE parent;
	size_t index;
	static size_t count;
public:
	Element();
	auto setParent(spE _parent);
	spE getParent();
	virtual void setIndex(size_t& _index) { index = _index; _index++; }
	size_t getIndex() const { return index; }
	virtual std::string getText() { return ""; };
	//Block settings
	virtual void setAlignment(std::string) {};
	virtual std::string getAlignment() { return "left"; };
	virtual void setBorder(std::string) {};
	virtual std::string getBorder() { return ""; };
	virtual void setMargin(std::string) {};
	virtual std::string getMargin(size_t) { return ""; };
	virtual void setPadding(std::string) {};
	virtual std::string getPadding(size_t) { return ""; };
	virtual void setMask(std::string) {};
	virtual std::string getMask() { return ""; };
	virtual void setWidth(std::string) {};
	virtual size_t getWidth() { return 0; };
	//
	virtual void insertBlock(spE) {};
	virtual void insertUpdated(spE, spE) {};
	virtual spE createElement(std::string, size_t) { return nullptr; };
	virtual void removeElement() {};
	virtual spE getByIndex(size_t _index);
	virtual size_t getCleanWidth() const { return 0; }
	virtual spE clone() const { return std::make_shared<Element>(*this); };
	virtual void removeFromBlock(spE) {};
	virtual void clear() {};
	virtual void render(std::string&) {};
	virtual void copy(spE) {};
	virtual spE getInner() { return nullptr; };
};

class Text : public Element {
private:
	std::string textItSelf{ "" };
public:
	Text(const std::string& _text) : textItSelf(_text) {};
	Text(const Text& that) = default;
	Text(std::string&& _text) : textItSelf(std::move(_text)) {};
	std::string getText() override { return textItSelf; }

	void render(std::string&) override;
	void removeElement() override;
	spE clone() const override { return std::make_shared<Text>(*this); }
	void copy(spE) override;
};

struct Border {
	size_t width;
	std::string type;
};


class Block : public Element {
private:
	enum class alignmentT {
		left,
		right,
		center
	};
	alignmentT alignment{ alignmentT::left };
	Border border{ 0, "" };
	std::array<size_t, 4> margin{ 0, 0, 0, 0 };
	std::array<size_t, 4> padding{ 0, 0, 0, 0 };
	std::string mask{ "" };
	size_t blockWidth;
	std::list<spE> innerBlocks;
public:
	
	Block(size_t _width) : blockWidth(_width) { index = count; };
	Block(const Block& that) = default;
	void setAlignment(std::string) override;
	std::string getAlignment() override;
	void setBorder(std::string) override;
	std::string getBorder() override;
	void setMargin(std::string) override;
	std::string getMargin(size_t) override;
	void setPadding(std::string) override;
	std::string getPadding(size_t) override;
	void setMask(std::string) override;
	std::string getMask() override;
	void setWidth(std::string) override;
	size_t getWidth() override { return blockWidth; }
	void insertBlock(spE) override;
	spE createElement(std::string, size_t) override;
	void removeElement() override;
	spE getByIndex(size_t) override;
	void setIndex(size_t&) override;
	spE clone() const override { return std::make_shared<Block>(*this); }
	void render(std::string&) override;
	size_t getCleanWidth() const override { return blockWidth - padding.at(1) - padding.at(3) - margin.at(1) - margin.at(3) - border.width * 2; }
	void removeFromBlock(spE) override ;
	void insertUpdated(spE, spE) override;
	void copy(spE) override;
	void clear() override;

	spE getInner() override;
	

};

#endif

