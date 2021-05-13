/**
 * \file components.cpp
 *
 * Ez a fájl tartalmazza az adatszerkezetet megvalósító osztályok tagfüggvényeinek megvalósítását
 */

#include "components.h"

void Components::Ingredient::printDetails( std::ostream& ostream ) const {
    ostream << name << " (" << unit << ")";
}

bool Components::Ingredient::operator==(const Ingredient &other) const {
    return getName() == other.getName();
}

String Components::Ingredient::getName() const {
    return this->name;
}

String Components::Ingredient::getUnit() const {
    return this->unit;
}

void Components::Ingredient::setName(String _name) {
    this->name = _name;
}

void Components::Ingredient::setUnit(String _unit) {
    this->unit = _unit;
}

void Components::IngredientQ::printDetails(std::ostream& ostream) const {
    ostream << name << " " << quantity << unit;
}

unsigned int Components::IngredientQ::getQuantity() const {
    return this->quantity;
}

void Components::IngredientQ::setQuantity(unsigned int _quantity) {
    this->quantity = _quantity;
}

void Components::Recipe::printDetails(std::ostream& ostream) const {
    ostream << title;
}

bool Components::Recipe::operator==(const Components::Recipe &other) const {
    return getTitle() == other.getTitle();
}

String Components::Recipe::getTitle() const {
    return this->title;
}

Components::LinkedList<Components::IngredientQ> *Components::Recipe::getIngredients() const {
    return this->ingredients;
}

Components::LinkedList<String> *Components::Recipe::getInstructions() const {
    return this->instructions;
}

void Components::Recipe::setTitle(const String &_title) {
    this->title = _title;
}

void Components::Recipe::setIngredients(Components::LinkedList<Components::IngredientQ> *_in) {
    this->ingredients = _in;
}

void Components::Recipe::setInstructions(Components::LinkedList<String> *_ins) {
    this->instructions = _ins;
}

Components::Recipe::~Recipe() {
    if ( ingredients != nullptr ) delete ingredients;
    if ( instructions != nullptr ) delete instructions;
}
