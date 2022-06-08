#include "element.h"

size_t Element::count = 0;

Element::Element() : parent(nullptr)
{
}

auto Element::setParent(spE _parent)
{
	parent = _parent;
}

spE Element::getParent()
{
	return parent;
}

spE Element::getByIndex(size_t _index)
{
	if (index == _index) {
		return shared_from_this();
	}
	return nullptr;
}



void Block::setAlignment(std::string alignmentStr)
{
	if (alignmentStr.find("align") != std::string::npos) {
		if (alignmentStr.find("left") != std::string::npos) {
			alignment = alignmentT::left;
		}
		else if (alignmentStr.find("right") != std::string::npos) {
			alignment = alignmentT::right;
		}
		else if (alignmentStr.find("center") != std::string::npos) {
			alignment = alignmentT::center;
		}
	}
	else {
		return;
	}
}

spE Block::getByIndex(size_t _index) {
	if (index == _index) {
		return shared_from_this();
	}
	else {
		for (const auto& element : innerBlocks) {
			if (element->getByIndex(_index) != nullptr) {
				return element->getByIndex(_index);
			}
			
		}
	}

	return nullptr;
}


void Block::setIndex(size_t& start) {
	index = start;
	start++;

	for (auto& element : innerBlocks) {
		element->setIndex(start);
	}
}

void Block::removeFromBlock(spE element)
{
	element->setParent(nullptr);
	innerBlocks.remove(element);
}

void Block::insertUpdated(spE before, spE what)
{
	std::list<spE>::iterator it;
	for (it = innerBlocks.begin(); it != innerBlocks.end(); it++) {
		if ((*it)->getIndex() == before->getIndex())
		{
			break;
		}
	}
	what->setParent(shared_from_this());
	innerBlocks.insert(it, what);
	before->removeElement();
}

void Block::setBorder(std::string borderStr)
{
	//#border 5 c
	if (borderStr.find("border") == std::string::npos) {
		return;
	}
	auto start = borderStr.find("border") + std::string("border").length() + 1;
	std::string borderWidth = "";
	for (; borderStr.at(start) != ' '; start++) {
		borderWidth += borderStr.at(start);
	}
	border.width = std::stoi(borderWidth);

	start = borderStr.find_last_of(" ") + 1;
	border.type.clear();
	for (; borderStr.at(start) != '\n' && start < borderStr.length(); start++) {
		border.type += borderStr.at(start);
	}
}

std::array<size_t, 4> fillByAttr(const std::string& String, const std::string& aim) {
	if (String.find(aim) == std::string::npos) {
		return { 0, 0, 0, 0 };
	}
	auto start = String.find(aim) + std::string(aim).length() + 1;
	std::array<size_t, 4> result;

	for (int i = 0; i < 4; i++) {
		std::string mValue = "";
		for (; start != String.length() && String.at(start) != '\n' && String.at(start) != ' '; start++) {
			mValue += String.at(start);
		}
		start++;
		result.at(i) = static_cast<size_t>(std::stoi(mValue));
	}
	return result;
}

void Block::setMargin(std::string marginStr)
{
	margin = fillByAttr(marginStr, "margin");
}

std::string Block::getMargin(size_t n)
{
	auto marginString = std::string("");
	for (auto i = 0; i < margin.at(n); i++) {
		marginString += ' ';
	}
	return marginString;
}

std::string Block::getPadding(size_t n)
{
	auto paddingString = std::string("");
	for (auto i = 0; i < padding.at(n); i++) {
		paddingString += ' ';
	}
	return paddingString;
}

std::string Block::getMask()
{
	return std::string(mask);
}

void Block::setPadding(std::string paddingStr) {
	padding = fillByAttr(paddingStr, "padding");
}

void Block::setMask(std::string maskStr) {
	if (maskStr.find("mask") == std::string::npos) {
		return;
	}
	//#mask f
	auto start = maskStr.find("mask") + std::string("mask").length() + 1;
	for (; maskStr.at(start) != '\n'; start++) {
		mask += maskStr.at(start);
	} 
}

void Block::setWidth(std::string _width) {
	if (_width.find("width") == std::string::npos) {
		return;
	}
	auto start = _width.find("width") + std::string("width").length() + 1;
	auto tempWidth = std::string();
	for (; start != _width.length() && _width.at(start) != '\n'; start++) {
		tempWidth += _width.at(start);
	}

	blockWidth = std::stoi(tempWidth);
}

void Block::insertBlock(spE element) {
	element->setParent(shared_from_this());
	innerBlocks.push_back(std::move(element));
}



spE Block::createElement(std::string currentBlock, size_t blockSize)
{
	auto openBrace = currentBlock.find("{");
	auto endBrace = currentBlock.find_last_of("}");


	auto contentBlock = currentBlock.substr(openBrace + 2, endBrace - openBrace + 2);

	auto begin = 0;

	spE current = std::make_shared<Block>(blockSize);

	auto currentPosition = 0;


	while (contentBlock.find("#", currentPosition) != std::string::npos && (contentBlock.find("#", currentPosition) < contentBlock.find("{", currentPosition))) {
		auto property = std::string();
		for (auto i = contentBlock.find("#", currentPosition); i <= contentBlock.find("\n", currentPosition); i++) {
			property += contentBlock.at(i);
		}
		currentPosition = contentBlock.find("\n", currentPosition) + 1;
		
		if (property.find("align") != std::string::npos) {
			current->setAlignment(property);
		}
		else if (property.find("margin") != std::string::npos) {
			current->setMargin(property);
		}
		else if (property.find("padding") != std::string::npos) {
			current->setPadding(property);
		}
		else if (property.find("mask") != std::string::npos) {
			current->setMask(property);
		}
		else if (property.find("border") != std::string::npos) {
			current->setBorder(property);
		}
		else if (property.find("width") != std::string::npos) {
			current->setWidth(property);
		}
	}


	while (currentPosition < contentBlock.length()) {
		if (contentBlock.at(currentPosition) == '$') {
			auto innerString = contentBlock.substr(currentPosition, contentBlock.find("\n", currentPosition) - currentPosition);
			
			current->insertBlock(std::make_shared<Text>(innerString));
			currentPosition = contentBlock.find("\n", currentPosition) + 1;
		}
		else if (contentBlock.at(currentPosition) == '{') {
			auto findEndBrace = [currentPosition, &contentBlock]() {
				auto endBrace = contentBlock.find("}", currentPosition);
				auto intermediateBraces = 0;
				for (auto i = currentPosition + 1; i < endBrace; i++) {
					if (contentBlock.at(i) == '{') {
						intermediateBraces++;
					}
				}
				while (intermediateBraces != 0) {
					endBrace = contentBlock.find("}", endBrace + 1);
					intermediateBraces--;
				}
				return endBrace;
			};

			auto innerBlock = currentBlock.substr(currentPosition + 2, findEndBrace() - currentPosition + 2);
			current->insertBlock(createElement(innerBlock, current->getCleanWidth()));

			currentPosition = findEndBrace() + 2;
		}
		else {
			currentPosition++;
		}
	}
	
	return current;
}

std::string Block::getBorder()
{
	auto borderString = std::string();
	for (auto i = 0; i < border.width; i++) {
		borderString += border.type;
	}

	return borderString;
}

std::string Block::getAlignment() {
	switch (alignment)
	{
	case alignmentT::left:
		return "left";
	case alignmentT::right:
		return "right";
	case alignmentT::center:
		return "center";
	default:
		break;
	}
}

void Block::render(std::string& result)
{

	auto findPart = [this]() {
		auto buffer = getParent();
		auto leftPart = std::string();
		auto rightPart = std::string();
		while (buffer != nullptr) {
			auto innerBuffer = buffer->getParent();
			auto difference = std::string();
			if (innerBuffer != nullptr) {
				for (auto i = 0; i < innerBuffer->getCleanWidth() - buffer->getWidth(); i++) {
					difference += ' ';
				}
				if (innerBuffer->getAlignment() == "right") {
					leftPart = difference + buffer->getMargin(3) + buffer->getBorder() + buffer->getPadding(3) + leftPart;
					rightPart = rightPart + buffer->getPadding(1) + buffer->getBorder() + buffer->getMargin(1);
				}
				else if (innerBuffer->getAlignment() == "center") {
					leftPart = difference.substr(difference.length() / 2) + buffer->getMargin(3) + buffer->getBorder()
						+ buffer->getPadding(3) + leftPart;
					rightPart = rightPart + buffer->getPadding(1) + buffer->getBorder() + buffer->getMargin(1)
						+ difference.substr(difference.length() / 2);
				}
				else {
					leftPart = buffer->getMargin(3) + buffer->getBorder() + buffer->getPadding(3) + leftPart;
					rightPart = rightPart + buffer->getPadding(1) + buffer->getBorder() + buffer->getMargin(1) + difference;
				}
			}
			else {
				leftPart = buffer->getMargin(3) + buffer->getBorder() + buffer->getPadding(3) + leftPart;
				rightPart = rightPart + buffer->getPadding(1) + buffer->getBorder() + buffer->getMargin(1) + difference;
			}
			buffer = buffer->getParent();
		}

		return std::pair<std::string, std::string>(leftPart, rightPart);
	};

	auto difference = std::string("");
	auto rootWidth = 100;
	if (getParent() != nullptr) {
		rootWidth = getParent()->getCleanWidth();
	}
	for (auto i = 0; i < rootWidth - getWidth(); i++) {
		difference += ' ';
	}
	
	//Top margin
	if (getParent() != nullptr) {
		if (getParent()->getAlignment() == "right") {
			for (auto i = 0; i < margin.at(0); i++) {
				result += findPart().first;
				result += difference;
				result += getMargin(3);
				for (auto j = 0; j < blockWidth - margin.at(3) - margin.at(1); j++) {
					result += " ";
				}
				result += getMargin(1);
				result += findPart().second;
				result += "\n";
			}
		}
		else if (getParent()->getAlignment() == "center") {
			for (auto i = 0; i < margin.at(0); i++) {
				result += findPart().first;
				result += difference.substr(0, difference.length() / 2);
				result += getMargin(3);
				for (auto j = 0; j < blockWidth - margin.at(3) - margin.at(1); j++) {
					result += " ";
				}
				result += getMargin(1);
				result += difference.substr(difference.length() / 2);
				result += findPart().second;
				result += "\n";
			}
		}
		else {
			for (auto i = 0; i < margin.at(0); i++) {
				result += findPart().first;
				result += getMargin(3);
				for (auto j = 0; j < blockWidth - margin.at(3) - margin.at(1); j++) {
					result += " ";
				}
				result += getMargin(1);
				result += difference;
				result += findPart().second;
				result += "\n";
			}
		}
	}
	else {
		for (auto i = 0; i < margin.at(0); i++) {
			result += findPart().first;
			result += getMargin(3);
			for (auto j = 0; j < blockWidth - margin.at(3) - margin.at(1); j++) {
				result += " ";
			}
			result += getMargin(1);
			result += difference;
			result += findPart().second;
			result += "\n";
		}
	}
	//
	// 
	//Top Border
	if (getParent() != nullptr) {
		if (getParent()->getAlignment() == "right") {
			for (auto i = 0; i < border.width; i++) {
				result += findPart().first + difference;
				result += getMargin(3);
				for (auto j = 0; j < blockWidth - margin.at(3) - margin.at(1); j++) {
					result += border.type;
				}
				result += getMargin(1);
				result += findPart().second;
				result += "\n";
			}
		}
		else if (getParent()->getAlignment() == "center") {
			for (auto i = 0; i < border.width; i++) {
				result += findPart().first + difference.substr(0, difference.length() / 2);
				result += getMargin(3);
				for (auto j = 0; j < blockWidth - margin.at(3) - margin.at(1); j++) {
					result += border.type;
				}
				result += getMargin(1);
				result += difference.substr(difference.length() / 2) + findPart().second;
				result += "\n";
			}
		}
		else {
			for (auto i = 0; i < border.width; i++) {
				result += findPart().first;
				result += getMargin(3);
				for (auto j = 0; j < blockWidth - margin.at(3) - margin.at(1); j++) {
					result += border.type;
				}
				result += getMargin(1);
				result += difference + findPart().second;
				result += "\n";
			}
		}
	}
	else {
		for (auto i = 0; i < border.width; i++) {
			result += findPart().first;
			result += getMargin(3);
			for (auto j = 0; j < blockWidth - margin.at(3) - margin.at(1); j++) {
				result += border.type;
			}
			result += getMargin(1);
			result += difference + findPart().second;
			result += "\n";
		}
	}

	//

	//Top Padding
	if (getParent() != nullptr) {
		if (getParent()->getAlignment() == "right") {
			for (auto i = 0; i < padding.at(0); i++) {
				result += findPart().first + difference;
				result += getMargin(3);
				result += getBorder();
				for (auto j = 0; j < blockWidth - margin.at(3) - margin.at(1) - 2 * border.width; j++) {
					result += " ";
				}
				result += getBorder();
				result += getMargin(1);
				result += findPart().second;
				result += "\n";
			}
		}
		else if (getParent()->getAlignment() == "center") {
			for (auto i = 0; i < padding.at(0); i++) {
				result += findPart().first + difference.substr(0, difference.length() / 2);
				result += getMargin(3);
				result += getBorder();
				for (auto j = 0; j < blockWidth - margin.at(3) - margin.at(1) - 2 * border.width; j++) {
					result += " ";
				}
				result += getBorder();
				result += getMargin(1);
				result += difference.substr(difference.length() / 2) + findPart().second;
				result += "\n";
			}
		}
		else {
			for (auto i = 0; i < padding.at(0); i++) {
				result += findPart().first;
				result += getMargin(3);
				result += getBorder();
				for (auto j = 0; j < blockWidth - margin.at(3) - margin.at(1) - 2 * border.width; j++) {
					result += " ";
				}
				result += getBorder();
				result += getMargin(1);
				result += difference + findPart().second;
				result += "\n";
			}
		}
	}
	else {
		for (auto i = 0; i < padding.at(0); i++) {
			result += findPart().first;
			result += getMargin(3);
			result += getBorder();
			for (auto j = 0; j < blockWidth - margin.at(3) - margin.at(1) - 2 * border.width; j++) {
				result += " ";
			}
			result += getBorder();
			result += getMargin(1);
			result += difference + findPart().second;
			result += "\n";
		}
	}
	
	//

	
	for (const auto& element : innerBlocks) {
		element->render(result);
	}

	//Bottom Padding
	if (getParent() != nullptr) {
		if (getParent()->getAlignment() == "right") {
			for (auto i = 0; i < padding.at(2); i++) {
				result += findPart().first + difference;
				result += getMargin(3);
				result += getBorder();
				for (auto j = 0; j < blockWidth - margin.at(3) - margin.at(1) - 2 * border.width; j++) {
					result += " ";
				}
				result += getBorder();
				result += getMargin(1);
				result += findPart().second;
				result += "\n";
			}
		}
		else if (getParent()->getAlignment() == "center") {
			for (auto i = 0; i < padding.at(2); i++) {
				result += findPart().first + difference.substr(0, difference.length() / 2);
				result += getMargin(3);
				result += getBorder();
				for (auto j = 0; j < blockWidth - margin.at(3) - margin.at(1) - 2 * border.width; j++) {
					result += " ";
				}
				result += getBorder();
				result += getMargin(1);
				result += difference.substr(difference.length() / 2) + findPart().second;
				result += "\n";
			}
		}
		else {
			for (auto i = 0; i < padding.at(2); i++) {
				result += findPart().first;
				result += getMargin(3);
				result += getBorder();
				for (auto j = 0; j < blockWidth - margin.at(3) - margin.at(1) - 2 * border.width; j++) {
					result += " ";
				}
				result += getBorder();
				result += getMargin(1);
				result += difference + findPart().second;
				result += "\n";
			}
		}
	}
	else {
		for (auto i = 0; i < padding.at(2); i++) {
			result += findPart().first;
			result += getMargin(3);
			result += getBorder();
			for (auto j = 0; j < blockWidth - margin.at(3) - margin.at(1) - 2 * border.width; j++) {
				result += " ";
			}
			result += getBorder();
			result += getMargin(1);
			result += difference + findPart().second;
			result += "\n";
		}
	}
	
	//
	//Bottom Border
	if (getParent() != nullptr) {
		if (getParent()->getAlignment() == "right") {
			for (auto i = 0; i < border.width; i++) {
				result += findPart().first + difference;
				result += getMargin(3);
				for (auto j = 0; j < blockWidth - margin.at(3) - margin.at(1); j++) {
					result += border.type;
				}
				result += getMargin(1);
				result += findPart().second;
				result += "\n";
			}
		}
		else if (getParent()->getAlignment() == "center") {
			for (auto i = 0; i < border.width; i++) {
				result += findPart().first + difference.substr(0, difference.length() / 2);
				result += getMargin(3);
				for (auto j = 0; j < blockWidth - margin.at(3) - margin.at(1); j++) {
					result += border.type;
				}
				result += getMargin(1);
				result += difference.substr(difference.length() / 2) + findPart().second;
				result += "\n";
			}
		}
		else {
			for (auto i = 0; i < border.width; i++) {
				result += findPart().first;
				result += getMargin(3);
				for (auto j = 0; j < blockWidth - margin.at(3) - margin.at(1); j++) {
					result += border.type;
				}
				result += getMargin(1);
				result += difference + findPart().second;
				result += "\n";
			}
		}
	}
	else {
		for (auto i = 0; i < border.width; i++) {
			result += findPart().first;
			result += getMargin(3);
			for (auto j = 0; j < blockWidth - margin.at(3) - margin.at(1); j++) {
				result += border.type;
			}
			result += getMargin(1);
			result += difference + findPart().second;
			result += "\n";
		}
	}
	
	//Bottom Margin
	if (getParent() != nullptr) {
		if (getParent()->getAlignment() == "right") {
			for (auto i = 0; i < margin.at(2); i++) {
				result += findPart().first + difference;
				for (auto j = 0; j < blockWidth; j++) {
					result += " ";
				}
				result += findPart().second;
				result += "\n";
			}
		}
		else if (getParent()->getAlignment() == "center") {
			for (auto i = 0; i < margin.at(2); i++) {
				result += findPart().first + difference.substr(0, difference.length() / 2);
				for (auto j = 0; j < blockWidth; j++) {
					result += " ";
				}
				result += difference.substr(difference.length() / 2) + findPart().second;
				result += "\n";
			}
		}
		else {
			for (auto i = 0; i < margin.at(2); i++) {
				result += findPart().first;
				for (auto j = 0; j < blockWidth; j++) {
					result += " ";
				}
				result += difference + findPart().second;
				result += "\n";
			}
		}
	}
	else {
		for (auto i = 0; i < margin.at(2); i++) {
			result += findPart().first;
			for (auto j = 0; j < blockWidth; j++) {
				result += " ";
			}
			result += difference + findPart().second;
			result += "\n";
		}
	}
	
	//



}


void Text::render(std::string& result)
{
	auto findPart = [this]() {
		auto buffer = getParent();
		auto leftPart = std::string();
		auto rightPart = std::string();
		while (buffer != nullptr) {
			auto innerBuffer = buffer->getParent();
			auto difference = std::string();
			if (innerBuffer != nullptr) {
				for (auto i = 0; i < innerBuffer->getCleanWidth() - buffer->getWidth(); i++) {
					difference += ' ';
				}
				if (innerBuffer->getAlignment() == "right") {
					leftPart = difference + buffer->getMargin(3) + buffer->getBorder() + buffer->getPadding(3) + leftPart;
					rightPart = rightPart + buffer->getPadding(1) + buffer->getBorder() + buffer->getMargin(1);
				}
				else if (innerBuffer->getAlignment() == "center") {
					leftPart = difference.substr(difference.length() / 2) + buffer->getMargin(3) + buffer->getBorder()
						+ buffer->getPadding(3) + leftPart;
					rightPart = rightPart + buffer->getPadding(1) + buffer->getBorder() + buffer->getMargin(1)
						+ difference.substr(difference.length() / 2);
				}
				else {
					leftPart = buffer->getMargin(3) + buffer->getBorder() + buffer->getPadding(3) + leftPart;
					rightPart = rightPart + buffer->getPadding(1) + buffer->getBorder() + buffer->getMargin(1) + difference;
				}
			}
			else {
				leftPart = buffer->getMargin(3) + buffer->getBorder() + buffer->getPadding(3) + leftPart;
				rightPart = rightPart + buffer->getPadding(1) + buffer->getBorder() + buffer->getMargin(1) + difference;
			}
			buffer = buffer->getParent();
		}

		return std::pair<std::string, std::string>(leftPart, rightPart);
	};

	
	auto buffer = getParent();
	while (buffer != nullptr) {
		if (buffer->getMask() != "") {
			auto length = textItSelf.length();
			textItSelf.clear();
			for (auto i = 0; i < length; i++) {
				textItSelf += buffer->getMask();
			}
		}
		buffer = buffer->getParent();
	}
	auto textBuffer = textItSelf;
	auto difference = std::string();
	for (auto i = 0; i < getParent()->getCleanWidth() - textItSelf.length(); i++) {
		difference += ' ';
	}
	if (getParent()->getAlignment() == "left") {
		textBuffer += difference;
	}
	else if (getParent()->getAlignment() == "right") {
		textBuffer = difference + textBuffer;
	}
	else if (getParent()->getAlignment() == "center") {
		textBuffer = difference.substr(0, difference.length() / 2) + textBuffer + difference.substr(difference.length() / 2);
	}

	result += findPart().first + textBuffer + findPart().second + "\n";
}

void Text::removeElement()
{
	getParent()->removeFromBlock(shared_from_this());
}

void Text::copy(spE destination)
{
	auto cloned = clone();
	destination->insertBlock(cloned);
}

void Block::clear() {
	innerBlocks.clear();
}

void Block::copy(spE destination)
{
	auto cloned = clone();
	cloned->clear();
	for (const auto& element : innerBlocks) {
		element->copy(cloned);
	}

	if (destination == nullptr) {
		destination = cloned;
	}
	else {
		destination->insertBlock(cloned);
	}
	
}

spE Block::getInner() {
	return *(innerBlocks.begin());
}

void Block::removeElement()
{
	getParent()->removeFromBlock(shared_from_this());
}
