#include <iostream>
#include "controller.h"
/**
 * \file main.cpp
 *
 * A program felhasználói interfésze
 * Itt valósul meg a menürendszer, ami segíti a program egyszerű használatát a felhasználónak
 */

using namespace Components;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;

/// A controller függvényeire mutató pointer
/// Menürendszer megvalósításához szükséges
typedef void (Controller::*MenuItem)();

/**
 * Menu osztály
 * A menüvezérlést megvalósító osztály
 * Segítségével letisztult, átlátható és könnyedén bővíthatő a program menürendszere
 * A könnyebb használhatóság miatt, és mivel a funkcionalitás nem igényli, ezért minden
 * adattag publikus láthatóságú
 */
class Menu {
public:
    int order;      /// Menüpont sorszáma - a fő és almenü közti váltás kivitelezéséhez szükséges
    String name;    /// Menüpont címe - Ezt írjuk ki a konzolra
    MenuItem ptr;   /// A függvényre mutató pointer

    /// Default konstruktor
    /// Létrehozza a menüpontot
    /// @param e - menüpont sorszáma
    /// @param n - menüpont címe
    /// @param p - függvénypointer
    Menu( int e, const char* n, MenuItem p ) :order( e ), name( n ), ptr( p ) {};

    /// Végjel konstruktor
    /// A működés miatt szükséges - a függvénypointer null-lal, a sorszámot 0-val inicializálja
    Menu() :ptr( nullptr ), order(0) {};
};


int main()
{
    cout << "NHF - Recepteskonyv" << endl;

    // Példányosítjuk a vezérlő osztályt
    Controller controller = Controller();

    // Menüpontokat tároló tömb
    Menu menupontok[] = {
            // Főmenü
            Menu( 100, "Alapanyaglista", nullptr ),
            Menu( 300, "Receptkonyv", nullptr ),
            Menu( 400, "Kereses", nullptr ),
            Menu( 200, "Kamra", nullptr ),
            // Alapanyaglista menü
            Menu( 10, "Alapanyag - Listazas", &Controller::listIngredients ),
            Menu( 10, "Alapanyag - Uj elem", &Controller::addIngredient ),
            Menu( 10, "Alapanyag - Eltavolitas", &Controller::removeIngredient ),
            Menu( 10, "Alapanyag - Modositas", &Controller::modifyIngredient ),
            // Kamra menü
            Menu( 20, "Kamra - Listazas", &Controller::listPantry ),
            Menu( 20, "Kamra - Uj elem", &Controller::addPantry ),
            Menu( 20, "Kamra - Eltavolitas", &Controller::removePantry ),
            Menu( 20, "Kamra - Modositas", &Controller::modifyPantry ),
            // Receptkönyv menü
            Menu( 30, "Receptkonyv - Listazas", &Controller::listRecipes ),
            Menu( 30, "Receptkonyv - Megtekint", &Controller::displayRecipe ),
            Menu( 30, "Receptkonyv - Uj elem", &Controller::addRecipe ),
            Menu( 30, "Receptkonyv - Eltavolitas", &Controller::removeRecipe ),
            Menu( 30, "Receptkonyv - Modositas", &Controller::modifyRecipe ),
            // Keresés menü
            Menu( 40, "Kereses - Etel neve alapjan", &Controller::searchByRecipeName ),
            Menu( 40, "Kereses - Nincs otletem", &Controller::searchRandom ),
            Menu( 40, "Kereses - Ennek egy kis...", &Controller::searchByOneIngredient ),
            Menu( 40, "Kereses - El kell hasznalni", &Controller::serachByMoreIngredient ),
            // Végjel
            Menu()
    };

    String  title("Fomenu");    // Aktuális menü címe
    bool    mainmenu = true;    // Az aktuális menü szintje
    int     submenu = 0;        // Az almenü számozása

    // Fő cíklus - pörgeti a menüpontokat
    while ( true )
    {
        std::string buffer; // beolvasáshoz szükséges ideiglenes tároló
        int menu_size;      // a menü teljes hossza
        int actual_size;    // az aktuális menü hossza
        int selected;       // kiválasztott elem

        cout << "[Menu - " << title << "]" << endl;

        // Kiírjuk a menüpontokat
        for ( menu_size = 1, actual_size = 1; menupontok[menu_size-1].order != 0; menu_size++ )
        {
            // Főmenü kiírása
            if ( mainmenu && (menupontok[menu_size-1].order/100 > 0) )
            {
                cout << actual_size << ". " << menupontok[menu_size-1].name << endl;
                actual_size++;
            }

            // Almenü kiírása
            if ( !mainmenu && submenu != 0 && menupontok[menu_size-1].order == submenu )
            {
                cout << actual_size << ". " << menupontok[menu_size-1].name << endl;
                actual_size++;
            }
        }

        // Ha a főmenüben vagyunk, az utolsó elem a kilépés
        // almenü esetén visszalépés a főmenübe
        cout << actual_size << (mainmenu ? ". Kilepes" : ". Visszalepes") << endl << "> ";

        // Felhasználói input bekérése
        std::getline( cin, buffer );

        try {
            selected = std::stoi( buffer ); // Számmá alakítjuk az inputot
            if ( selected < 1 || selected > actual_size ) throw std::exception(); // A kapott input szerepel-e a választhatóak között?
        }
        catch( std::invalid_argument& e ) { cerr << "Hibas bemenet!" << endl; continue; } // Hibás bemenetet kaptunk
        catch( std::exception& e ) { cerr << "Nincs ilyen menupont!" << endl; continue; } // Nincs ilyen menüpont

        if ( mainmenu && selected == actual_size ) break; // Kilépés a programból
        else if ( !mainmenu && selected == actual_size ) // Visszalépés a főmenübe
        {
            title = "Fomenu";
            mainmenu = true;
            submenu = 0;
            cout << endl;
            continue;
        }

        cout << endl;

        if ( mainmenu ) // Almenü kiválasztása
        {
            submenu = menupontok[selected-1].order/10;
            mainmenu = false;
            title = menupontok[selected-1].name;
        }
        else // Válaszott menü lefuttatása, megkeressük a menüpontot, futtatjuk a feladatot
        {
            int current_step_counter = 1;
            for ( int i = 0; menupontok[i].order != 0; i++ )
            {
                if ( menupontok[i].order == submenu )
                {
                    if ( selected == current_step_counter )
                    {
                        (controller.*(menupontok[i].ptr))();
                        break;
                    }
                    else
                        current_step_counter++;
                }
            }
            cout << endl;
        }
    }

    return 0;
}
