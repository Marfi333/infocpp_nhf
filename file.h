#ifndef NHF4_FILE_H
#define NHF4_FILE_H
/**
 * \file file.h
 *
 * Ez a fájl tartalmazza a fájlból olvasáshoz, és fájlba íráshoz szükséges osztályokat
 */


#include <iostream>
#include <fstream>
#include "string5.h"
#include "list.h"
#include "components.h"
#include "memtrace.h"

namespace File
{
    /**
     * Writer osztály
     * Az adatszerkezet fájlba írását megvalósító osztály
     */
    class Writer
    {
    private:
        String path;    /// Fájl útvonala
        String buffer;  /// Buffer - parse-oláshoz szükséges ideiglenes tároló -> ez kerül kiírásra a fájlba

    public:
        /// Default konstruktor - inicializálja a fájl utvonalát
        /// @param p - a fájl útvonala
        explicit Writer( const String& p ) :path( p ) {};

        /// Kiírja a buffert a fájlba
        /// ofstream::failure hibát dob, ha nem sikerült a művelet
        void write();

        /// Parse függvények
        /// A paraméterben kapott listát írható formátumú szöveggé alakítja, majd menti a bufferbe
        /// @param input - a kiírni kívánt lista
        void parse( Components::LinkedList<Components::Recipe>& input );
        void parse( Components::LinkedList<Components::Ingredient>& input );
        void parse( Components::LinkedList<Components::IngredientQ>& input );
        void parse( Components::LinkedList<String>& input );
    };

    /**
     * Reader osztály
     * Az adatszerkezet fájlból visszatöltését megvalósító osztály
     */
    class Reader
    {
    private:
        String path;    /// Fájl útvonala
        Components::LinkedList<String> buffer;  /// Buffer - ideiglenes tároláshoz szükséges lista

    public:
        /// Default konstruktor - inicializálja a fájl útvonalát
        /// @param p - fájl útvonala
        explicit Reader( const String& p ) :path( p ), buffer(Components::LinkedList<String>()) {};

        /// Beolvassa az összes sort a megadott fájlból
        /// ifstream::failure hibát dob, ha nem sikerült a művelet
        void read();

        /// Parse függvények
        /// A paraméterben kapott listába tölti a beolvasott elemeket, egy séma alapján
        /// @param ing - lista referenciája, amibe betöltjük a beolvasott elemeket
        void parseIngredientQ( Components::LinkedList<Components::IngredientQ>& ing );
        void parseIngredient( Components::LinkedList<Components::Ingredient>& ing );
        void parseRecipe( Components::LinkedList<Components::Recipe>& ing );
    };
}

#endif //NHF4_FILE_H
