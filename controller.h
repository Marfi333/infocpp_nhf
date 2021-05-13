#ifndef NHF4_CONTROLLER_H
#define NHF4_CONTROLLER_H
/**
 * \file controller.h
 *
 * Ez a fájl tartalmazza a konzolos felhasználói felületet működtető osztályt
 */

#include "components.h"
#include "list.h"
#include "file.h"
#include "memtrace.h"
#include "string5.h"

/**
 * Controller osztály
 * Konzolos felhasználói felületet megvalósító osztály
 * Tartalmazza a függvényeket, melyekkel hozzáférhetünk és módosíthatjuk a fő adattagokat
 */
class Controller
{
private:
    Components::LinkedList<Components::Recipe> recipeList;          /// Receptlista
    Components::LinkedList<Components::Ingredient> ingredientList;  /// Alapanyaglista
    Components::LinkedList<Components::IngredientQ> pantryList;     /// Kamra lista

    /// Hozzávalólista módosítása - fő metódus (művelet kiválasztása)
    /// @param list - lista amiben módosítani szeretnénk
    /// @return bool - módosítás sikeressége
    bool modifyIngredientQ( Components::LinkedList<Components::IngredientQ>* list );

    /// Instrukciólista módosítása - fő metódus (művelet kiválasztása)
    /// @param list - lista amiben módosítani szeretnénk
    /// @return bool - módosítás sikeressége
    bool modifyStringList(Components::LinkedList<String>* list );


    /// Kiválasztott elem módosítása (Instrukciólista)
    /// @param list - lista amiben módosítani szeretnénk
    /// @return bool - módosítás sikeressége
    bool editStringList( Components::LinkedList<String>* list );

    /// Új elem hozzáadása (Instrukciólista)
    /// @param list - lista amiben módosítani szeretnénk
    /// @return bool - módosítás sikeressége
    bool addStringList( Components::LinkedList<String>* list );

    /// Kiválaszott elem eltávolítása (Instrukciólista)
    /// @param list - lista amiben módosítani szeretnénk
    /// @return bool - módosítás sikeressége
    bool removeStringList( Components::LinkedList<String>* list );


    /// Kiválasztott elem módosítása (Hozzávalólista)
    /// @param list - lista amiben módosítani szeretnénk
    /// @return bool - módosítás sikeressége
    bool editIngredientQList( Components::LinkedList<Components::IngredientQ>* list );

    /// Új elem hozzáadása (Hozzávalólista)
    /// @param list - lista amiben módosítani szeretnénk
    /// @return bool - módosítás sikeressége
    bool addIngredientQList( Components::LinkedList<Components::IngredientQ>* list );

    /// Kiválaszott elem eltávolítása (Hozzávalólista)
    /// @param list - lista amiben módosítani szeretnénk
    /// @return bool - módosítás sikeressége
    bool removeIngredientQList( Components::LinkedList<Components::IngredientQ>* list );


    /// Keresés eredményét megjelenítő függvény
    /// @param list - Result lista
    void displaySearchResult( Components::LinkedList<Components::Result<Components::Recipe>* >& list );
public:
    /// Default konstruktor
    /// Létrehozza az adatszerkezetet, beolvassa az előzőleg mentett adatokat a fájlokból
    Controller();

    /// Receptek kilistázása
    void listRecipes();

    /// Kiválasztott recept megtekintése
    void displayRecipe();

    /// Új recept hozzáadása
    void addRecipe();

    /// Kiválasztott recept törlése
    void removeRecipe();

    /// Kiválasztott recept szerkesztése
    void modifyRecipe();


    /// Alapanyagok kilistázása
    void listIngredients();

    /// Új alapanyag hozzáadása
    void addIngredient();

    /// Kiválasztott alapanyag törlése
    void removeIngredient();

    /// Kiválasztott alapanyag módosítása
    void modifyIngredient();


    /// Kamra listázása
    void listPantry();

    /// Új kamra-elem hozzáadása
    void addPantry();

    /// Kiválasztott kamra-elem eltávolítása
    void removePantry();

    /// Kiválasztott kamra-elem módosítása
    void modifyPantry();

    /// Keresés a recept neve alapján
    void searchByRecipeName();

    /// Random recept keresése
    void searchRandom();

    /// Keresés egy hozzávaló alapján
    void searchByOneIngredient();

    /// Keresés több hozzávaló alapján
    void serachByMoreIngredient();

    /// Destruktor
    /// Menti az adatszerkezetet a fájlokba
    ~Controller();
};


#endif //NHF4_CONTROLLER_H
