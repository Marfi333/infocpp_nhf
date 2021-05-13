#ifndef NHF4_COMPONENTS_H
#define NHF4_COMPONENTS_H
/**
 * \file components.h
 *
 * Ez a fájl tartalmazza az adatszerkezetet megvalósító osztályokat
 */

#include "./memtrace.h"
#include "./string5.h"
#include "./list.h"


/// Inline függvény, a paraméterként adott szting elejéről eltávolítja az összes space-t
/// @param s - módosítandó sztring
/// @return a módosított sztring referenciája
inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

/// Inline függvény, a paraméterként adott szting végéről eltávolítja az összes space-t
/// @param s - módosítandó sztring
/// @return a módosított sztring referenciája
inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

/// Inline függvény, a paraméterként adott szting minkét oldaláról eltávolítja az összes space-t
/// @param s - módosítandó sztring
/// @return a módosított sztring referenciája
inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}


namespace Components
{
    /**
     * Ingredient osztály
     * Alapanyagokat tároló osztály
     */
    class Ingredient
    {
    protected:
        String name;    /// Alapanyag neve
        String unit;    /// Alapanyag mértékegysége

    public:
        /// Konstruktor
        /// Inicializálja az alapanyag nevét és mértékegységét
        /// @param _name - alapanyag neve
        /// @param _unit - alapanyag mértékegysége
        Ingredient( String _name, String _unit ) :name(_name), unit(_unit) {};

        /// Default konstruktor
        Ingredient() {};

        /// Alapanyag neve getter
        /// @return String - alapayag neve
        String getName() const;

        /// Alapanyag mértékegysége getter
        /// @return String - alapanyag mértékegysége
        String getUnit() const;

        /// Alapanyag neve setter
        /// @param _name - név
        void setName( String _name );

        /// Alapanyag mértékegysége setter
        /// @param _unit - mértékegység
        void setUnit( String _unit );

        /// Kiírja az alapanyag adatait a megadott standard outputra
        /// @param ostream - standard output
        virtual void printDetails( std::ostream& ostream ) const;

        /// operator ==
        /// Megvizsgálja, hogy a megadott alapanyag (kifejezés jobb oldala) megegyezik-e ezzel
        /// @param other - vizsgálandó alapanyag (kif. jobb oldala)
        /// @return bool - egyeznek-e
        bool operator==( const Ingredient& other ) const;

        /// Virtuális destruktor
        virtual ~Ingredient() {};
    };

    /**
     * IngredientQ osztály
     * Alapanyagot és a hozzá tartozó mennyiséget tároló osztály
     */
    class IngredientQ : public Ingredient
    {
    protected:
        unsigned int quantity; /// mennyiség

    public:
        /// Konstruktor
        /// Inicializálja az ős osztályt, és a mennyiséget
        /// @param _name - alapanyag neve
        /// @param _unit - alapanyag mennyisége
        /// @param _quantity - alapanyag mennyisége
        IngredientQ( String _name, String _unit, unsigned int _quantity ) :Ingredient(_name, _unit), quantity(_quantity) {};

        /// Default konstruktor
        IngredientQ() {};

        /// Mennyiség getter
        /// return unsigned int - alapanyag mennyisége
        unsigned int getQuantity() const;

        /// Mennyiség setter
        /// @param _quantity - mennyiség
        void setQuantity( unsigned int _quantity );

        /// Kiírja az alapanyag adatait a megadott standard outputra
        /// @param ostream - standard output
        void printDetails( std::ostream& ostream ) const;

        /// Destruktor
        ~IngredientQ() {};
    };

    /**
     * Recipe osztály
     * Recepteket tároló osztály
     */
    class Recipe
    {
    private:
        String title;   /// Recept neve
        LinkedList<IngredientQ>* ingredients;   /// Alapanyaglista
        LinkedList<String>* instructions;       /// Instrukció-lista

    public:
        /// Explicit Konstruktor
        /// null-lal inicializálja a listákat
        explicit Recipe() :title(), ingredients( nullptr ), instructions( nullptr ) {};

        /// Konstruktor
        /// Inicializálja a recept nevét, és a listákat
        /// @param tit - recept neve
        /// @param inst - instrukciólista pointere
        /// @param ing - alapanyaglista pointere
        Recipe( const String& tit, LinkedList<IngredientQ>* ing, LinkedList<String>* inst )
            :title( tit ), ingredients( ing ), instructions( inst ) {};

        /// Recept név getter
        /// @return String - recept neve
        String getTitle() const;

        /// Alapanyaglista getter
        /// @return alapanyaglistára mutató pointer
        LinkedList<IngredientQ>* getIngredients() const;

        /// Instrukciólista getter
        /// @return instrukciólistára mutató pointer
        LinkedList<String>* getInstructions() const;

        /// Recept név setter
        /// @param _title - név
        void setTitle( const String& _title );

        /// Alapanyaglista setter
        /// @param _in - listára mutató pointer
        void setIngredients( LinkedList<IngredientQ>* _in );

        /// Instrukciólista setter
        /// @param _ins - listára mutató pointer
        void setInstructions( LinkedList<String>* _ins );

        /// Kiírja a recept adatait a megadott standard outputra
        /// @param ostream - standard output
        void printDetails( std::ostream& ostream ) const;

        /// operator==
        /// Megvizsgálja, hogy a kifejezés jobb oldalán lévő receptnek a neve megegyezik-e ezzel
        /// @param other - kif. jobb oldalán lévő recept
        /// @return bool - egyeznek-e
        bool operator==( const Recipe& other ) const;

        /// Destruktor
        /// Felszabadítja a listákat
        ~Recipe();
    };
}

#endif // NHF4_COMPONENTS_H