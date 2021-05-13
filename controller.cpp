/**
 * \file controller.cpp
 *
 * Ez a fájl tartalmazza a konzolos felhasználói felületet működtető osztály megvalósítását
 */

#include "controller.h"

#include <algorithm>
#include <iostream>
#include "file.h"
#include "memtrace.h"
#include "components.h"

using namespace File;
using namespace Components;
using std::cout;
using std::endl;
using std::cerr;
using std::stringstream;


// Keresést megvalósító funktorok

/**
 * title_contains funktor
 * generikus kereséshez szükséges
 * a recept címében keresi meg a megadott szövegrészletet
 */
class title_contains
{
    String compare; /// Megadott szövegrészlet
public:
    /// Konstruktor
    /// Inicializálja a keresett szöveget
    title_contains(String const &cmp): compare(cmp) {};

    /// operator()
    /// szerepel-e a címben a keresett szöveg
    /// @param item - aktuális recept
    /// @return bool - sikeres találat esetén igaz
    bool operator()(Recipe const &item) const
    {
        String title( item.getTitle() );
        title.toLower();
        String comp( compare );
        comp.toLower();

        return title.find(comp);
    }
};

/**
 * ingredient_contains funktor
 * generikus kereséshez szükséges
 * a recept hozzávalólistájában keresi meg a megadott alapanyagokat
 */
class ingredient_contains
{
    LinkedList<Ingredient>& ingredients; /// Keresett alapanyagok
public:
    /// Konstruktor
    /// Inicializáljuk a keresett alapanyagokat
    ingredient_contains( LinkedList<Ingredient>& list ) :ingredients( list ) {};

    /// operator()
    /// a megadott alapanyaglista összes elemét megkeresi az aktuális recept hozzávalói között
    /// @param item - aktuális recept
    /// @return bool - sikeres találat esetén igaz
    bool operator()(Recipe const &item) const
    {
        LinkedList<Ingredient>::Iterator start = ingredients.begin();
        for ( ; start != ingredients.end(); start++ )
        {
            IngredientQ* element = (IngredientQ*)(&*start);
            if ( !item.getIngredients()->contains( element ) ) return false;
        }
        return true;
    }
};


Controller::Controller()
    :ingredientList( LinkedList<Ingredient>() ),
     pantryList( LinkedList<IngredientQ>() ),
     recipeList( LinkedList<Recipe>() )
{
    Reader recipeReader = Reader( "recipes.dat" );
    try {
        recipeReader.read();
        recipeReader.parseRecipe( recipeList );
    } catch ( std::ifstream::failure& ex ) { cerr << ex.what() << endl; }

    Reader ingredientReader = Reader( "ingredients.dat" );
    try {
        ingredientReader.read();
        ingredientReader.parseIngredient( ingredientList );
    } catch ( std::ifstream::failure& ex ) { cerr << ex.what() << endl; }

    Reader pantryReader = Reader( "pantry.dat" );
    try {
        pantryReader.read();
        pantryReader.parseIngredientQ( pantryList );
    } catch ( std::ifstream::failure& ex ) { cerr << ex.what() << endl; }
}

Controller::~Controller() {
    int success = 0;

    Writer recipeWriter = Writer( "recipes.dat" );
    try {
        recipeWriter.parse( recipeList );
        recipeWriter.write();
        success++;
    } catch ( std::ofstream::failure& ex ) { cerr << ex.what() << endl; }

    Writer ingredientWriter = Writer( "ingredients.dat" );
    try {
        ingredientWriter.parse( ingredientList );
        ingredientWriter.write();
        success++;
    } catch ( std::ofstream::failure& ex ) { cerr << ex.what() << endl; }

    Writer pantryWriter = Writer( "pantry.dat" );
    try {
        pantryWriter.parse( pantryList );
        pantryWriter.write();
        success++;
    } catch ( std::ofstream::failure& ex ) { cerr << ex.what() << endl; }

    success == 3 ? cout << endl << "[Az adatok mentesre kerultek a fajlokba]" : cout << endl << "[Hiba tortent az adatok mentese soran]";
}

// Publikus metódusok
void Controller::listRecipes() {
    cout << "[Receptek listazasa]" << endl;
    recipeList.printOrderedList( cout, true );
}
void Controller::displayRecipe() {
    cout << "[Recept megtekintese]" << endl;
    if ( recipeList.size() < 1 ) { cout << "A  receptes lista ures!" << endl; return; }

    std::string buffer;
    cout << "Hanyadik receptet akarod megtekinteni? ";
    std::getline( std::cin, buffer );

    int item;
    try {
        item = (std::stoi( buffer ))-1;
        if ( recipeList.size() < 1 || ( item < 0 || item >= recipeList.size() ) ) throw std::out_of_range( "hibas elem" );
    }
    catch ( std::invalid_argument& ex ) { cerr << "Hibas szam! Kapott input: \"" + buffer + "\"" << endl; return; }
    catch ( std::out_of_range& ex ) { cerr << "Nem talalhato a megadott elem! Kapott input: \"" + buffer + "\"" << endl; return; };

    Recipe* recipe;
    try {
        recipe = recipeList.get( item );
    } catch( std::exception& ex ) { cerr << "A megadott elem nem szerepel a listaban! Kapott input: \"" << item << "\""; return; }

    cout << String("[") + recipe->getTitle() + "]\nHozzavalok: " << endl;
    recipe->getIngredients()->printOrderedList( cout, true );

    cout << "Instrukciok: " << endl;
    recipe->getInstructions()->printOrderedList( cout, true );
}
void Controller::addRecipe() {
    cout << "[Recept hozzadasa]" << endl;

    Recipe* current = new Recipe();
    std::string buffer;

    cout << "Recept neve: ";
    std::getline( std::cin, buffer );
    if ( trim( buffer ).size() < 1 ) { cerr << "Hibas recept nev!" << endl; delete current; return; }

    current->setTitle( buffer.c_str() );
    if ( recipeList.indexOf( current ) != -1 ) { cout << "A recept mar szerepel a listaban!" << endl; delete current; return; }

    cout << "Hozzavalok (formatum: (nev mertekegyseg mennyiseg), vesszovel felsorolva): ";
    std::getline( std::cin, buffer );

    LinkedList<IngredientQ>* ingredients = new LinkedList<IngredientQ>();
    stringstream line( buffer );
    std::string segment;

    while ( std::getline( line, segment, ',' ) )
    {
        stringstream element( segment );
        std::vector<std::string> list;
        std::string piece;

        while ( std::getline( element, piece, ' ' ) )
        {
            list.push_back( piece );
        }

        if ( list.size() != 3 ) { cerr << "Nem megfelelo hozzavalo! Kapott input: \"" + element.str() + "\"" << endl; continue; }

        int number;
        try {
            number = std::stoi( list[2] );
        } catch ( std::invalid_argument& e ) { cerr << "Hibas szam! Kapott input: \"" + list[2] + "\"" << endl; continue; }

        IngredientQ c_ing = IngredientQ( String(list[0].c_str()), String(list[1].c_str()), number );
        if ( ingredients->indexOf( c_ing ) != -1 ) { cerr << "A megadott elem mar szerepel a listaban! Kapott input: \"" + list[0] + "\""; continue; }
        ingredients->push( c_ing );
    }
    current->setIngredients( ingredients );

    cout << "Instrukciok (vesszovel felsorolva): ";
    std::getline( std::cin, buffer );

    LinkedList<String>* instructions = new LinkedList<String>();
    stringstream i_line( buffer );
    std::string i_seg;

    while ( std::getline( i_line, i_seg, ',' ) )
    {
        instructions->push( String( i_seg.c_str() ) );
    }
    current->setInstructions( instructions );

    recipeList.push( *current );
    cout << "[Recepet sikeresen hozzaadva]" << endl;

    current->setIngredients( nullptr );
    current->setInstructions( nullptr );
    delete current;
}
void Controller::removeRecipe() {
    cout << "[Recept torlese]" << endl;
    if ( recipeList.size() < 1 ) { cout << "A receptes lista ures!" << endl; return; }
    cout << "Add meg hanyadik elemet szeretned torolni: ";

    std::string buffer;
    std::getline( std::cin, buffer );

    int number;
    try {
        number = std::stoi( buffer );
    } catch ( std::invalid_argument& ex ) { cerr << "Hibas szam!" << endl; return; };
    number--;

    try {
        recipeList.pop( number );
    } catch ( std::out_of_range& ex ) { cerr << "Nem talalhato a megadott elem! Kapott input: \"" + buffer + "\"" << endl; return; }

    cout << "[Recept sikeresen torolve]" << endl;
}
void Controller::modifyRecipe() {
    cout << "[Recept modositasa]" << endl;
    if ( recipeList.size() < 1 ) { cout << "A  receptes lista ures!" << endl; return; }

    std::string buffer;
    cout << "Hanyadik receptet akarod modositani? ";
    std::getline( std::cin, buffer );

    int item;
    try {
        item = (std::stoi( buffer ))-1;
        if ( recipeList.size() < 1 || ( item < 0 || item >= recipeList.size() ) ) throw std::out_of_range( "hibas elem" );
    }
    catch ( std::invalid_argument& ex ) { cerr << "Hibas szam! Kapott input: \"" + buffer + "\"" << endl; return; }
    catch ( std::out_of_range& ex ) { cerr << "Nem talalhato a megadott elem! Kapott input: \"" + buffer + "\"" << endl; return; };

    Recipe* selected = recipeList.get( item );

    cout << "1. Cim modositasa | 2. Hozzavalok modositasa | 3. Instrukciok modositasa | 4. Megse\nValassz muveletet: ";
    std::getline( std::cin, buffer );
    try {
        item = (std::stoi( buffer ));
        if ( item < 1 || item > 4 ) throw std::out_of_range( "hibas elem" );
    }
    catch ( std::invalid_argument& ex ) { cerr << "Hibas szam! Kapott input: \"" + buffer + "\"" << endl; return; }
    catch ( std::out_of_range& ex ) { cerr << "Nem talalhato a megadott elem! Kapott input: \"" + buffer + "\"" << endl; return; };

    switch ( item )
    {
        case 1: {
            cout << "Add meg a recept uj nevet: ";
            std::getline( std::cin, buffer );
            std::string tmp = buffer;
            std::string::iterator end_pos = std::remove(tmp.begin(), tmp.end(), ' ');
            tmp.erase(end_pos, tmp.end());

            if ( tmp.size() < 1 ) { cerr << "Hibas nev! Kapott input: \"" + buffer + "\"" << endl; return; }
            if ( recipeList.contains( Recipe(String(tmp.c_str()), nullptr, nullptr) ) ) { cerr << "A megadott nev foglalt!" << endl; cout << "[Recept modositasa sikertelen]" << endl; return; }
            selected->setTitle( String( buffer.c_str() ) );

        break;
        }
        case 2: {
            modifyIngredientQ( selected->getIngredients() ) ?
                cout << "[Recept sikeresen modositva]" << endl : cout << "[Recept modositasa sikertelen]" << endl;
        return;
        }
        case 3: {
            modifyStringList(selected->getInstructions()) ?
                cout << "[Recept sikeresen modositva]" << endl : cout << "[Recept modositasa sikertelen]" << endl;
        break;
        }
        case 4: {
            cout << "[Recept modositas visszavonva]" << endl;
        return;
        }
    }
}

void Controller::listIngredients() {
    cout << "[Alapanyagok listazasa]" << endl;
    ingredientList.printOrderedList( std::cout, true );
}
void Controller::addIngredient() {
    cout << "[Alapanyag hozzaadasa]\nAdd meg az uj alapanyagot (nev mertekegyseg): ";

    std::string buffer;
    std::getline( std::cin, buffer );

    stringstream line( buffer );
    std::string segment;
    std::vector<std::string> tmp;

    while ( std::getline( line, segment, ' ' ) )
    {
        tmp.push_back( segment );
    }

    if ( trim( buffer ).empty() || tmp.size() != 2 || tmp[0].empty() || tmp[1].empty() ) { cerr << "Hibas formatum! Kapott input: \"" + buffer + "\"" << endl; return; }
    int selected = ingredientList.indexOf( Ingredient( String(tmp[0].c_str()), "" ) );

    if ( selected != -1 )
    {
        cout << "A megadott alapanyag mar szerepel a listaban. A mertekegyseg opcionalisan felulirva!" << endl;
        ingredientList.get( selected )->setUnit( String(tmp[1].c_str()) );
        return;
    }

    ingredientList.push( Ingredient(String(tmp[0].c_str()), String(tmp[1].c_str())) );
    cout << "[Alapanyag sikeresen hozzaadva!]" << endl;
}
void Controller::removeIngredient() {
    cout << "[Alapanyag eltavolitasa]" << endl;
    if ( ingredientList.size() < 1 ) { cout << "A lista ures!" << endl; return; }
    cout << "Add meg hanyadik elemet szeretned torolni: ";

    std::string buffer;
    std::getline( std::cin, buffer );
    int selected;

    try {
        selected = std::stoi( buffer );
        ingredientList.get(--selected);
    }
    catch( std::invalid_argument& ex ) { cerr << "Hibas formatum! Kapott input: \"" + buffer + "\"" << endl; return; }
    catch( std::out_of_range& ex ) { cerr << "A megadott elem nem szerepel a listaban! Kapott input: \"" + buffer + "\"" << endl; return; }

    ingredientList.pop( selected );
    cout << "[Alapanyag sikeresen eltavolitva]" << endl;
}
void Controller::modifyIngredient() {
    cout << "[Alapanyag modositasa]" << endl;
    if ( ingredientList.size() < 1 ) { cout << "A lista ures!" << endl; return; }
    cout << "Add meg hanyadik elemet szeretned modositani: ";

    std::string buffer;
    std::getline( std::cin, buffer );
    int selected;

    try {
        selected = std::stoi( buffer );
        ingredientList.get(--selected);
    }
    catch( std::invalid_argument& ex ) { cerr << "Hibas formatum! Kapott input: \"" + buffer + "\"" << endl; return; }
    catch( std::out_of_range& ex ) { cerr << "A megadott elem nem szerepel a listaban! Kapott input: \"" + buffer + "\"" << endl; return; }

    cout << "Alapanyag uj neve (elozo eretek megtartasa eseten ures): ";
    std::getline( std::cin, buffer );
    trim( buffer );
    if ( !buffer.empty() )
    {
        if ( ingredientList.indexOf( Ingredient(String(buffer.c_str()), "") ) != -1 )
            cout << "A megadott alapanyag mar szerepel a listaban!" << endl;
        else
            ingredientList.get( selected )->setName( String(buffer.c_str()) );
    }

    cout << "Alapanyag uj m.egysege (elozo eretek megtartasa eseten ures): ";
    std::getline( std::cin, buffer );
    trim( buffer );
    if ( !buffer.empty() ) ingredientList.get( selected )->setUnit( String(buffer.c_str()) );

    cout << "[Alapanyag sikeresen modositva]" << endl;
}

void Controller::listPantry() {
    cout << "[Kamra listazasa]" << endl;
    pantryList.printOrderedList( cout, true );
}
void Controller::addPantry() {
    cout << "[Kamra alapanyag hozzaadasa]\nAdd meg az uj alapanyagot (nev mertekegyseg mennyiseg): ";

    std::string buffer;
    std::getline( std::cin, buffer );
    int number;

    stringstream line( buffer );
    std::string segment;
    std::vector<std::string> tmp;

    while ( std::getline( line, segment, ' ' ) )
    {
        tmp.push_back( segment );
    }

    if ( trim( buffer ).empty() || tmp.size() != 3 || tmp[0].empty() || tmp[1].empty() || tmp[2].empty() ) { cerr << "Hibas formatum! Kapott input: \"" + buffer + "\"" << endl; return; }
    try {
        number = std::stoi( tmp[2] );
    } catch( std::invalid_argument& ex ) { cerr << "Hibas formatum! Kapott input: \"" + buffer + "\"" << endl; return; }

    int selected = pantryList.indexOf( IngredientQ( String(tmp[0].c_str()), "", 0 ) );

    if ( selected != -1 )
    {
        cout << "A megadott alapanyag mar szerepel a listaban. A mertekegyseg es mennyiseg opcionalisan felulirva!" << endl;
        pantryList.get( selected )->setUnit( String( tmp[1].c_str() ) );
        pantryList.get( selected )->setQuantity( number );
        return;
    }

    pantryList.push( IngredientQ( String(tmp[0].c_str()), String(tmp[1].c_str()), number ) );
    cout << "[Kamra alapanyag sikeresen hozzaadva]" << endl;
}
void Controller::removePantry() {
    cout << "[Kamra alapanyag eltavolitasa]" << endl;
    if ( pantryList.size() < 1 ) { cout << "A lista ures!" << endl; return; }
    cout << "Add meg hanyadik elemet szeretned torolni: ";

    std::string buffer;
    std::getline( std::cin, buffer );
    int selected;

    try {
        selected = std::stoi( buffer );
        pantryList.get(--selected);
    }
    catch( std::invalid_argument& ex ) { cerr << "Hibas formatum! Kapott input: \"" + buffer + "\"" << endl; return; }
    catch( std::out_of_range& ex ) { cerr << "A megadott elem nem szerepel a listaban! Kapott input: \"" + buffer + "\"" << endl; return; }

    pantryList.pop( selected );
    cout << "[Kamra alapanyag sikeresen eltavolitva]" << endl;
}
void Controller::modifyPantry() {
    cout << "[Kamra alapanyag modositasa]" << endl;
    if ( pantryList.size() < 1 ) { cout << "A lista ures!" << endl; return; }
    cout << "Add meg hanyadik elemet szeretned modositani: ";

    std::string buffer;
    std::getline( std::cin, buffer );
    int selected;

    try {
        selected = std::stoi( buffer );
        pantryList.get(--selected);
    }
    catch( std::invalid_argument& ex ) { cerr << "Hibas formatum! Kapott input: \"" + buffer + "\"" << endl; return; }
    catch( std::out_of_range& ex ) { cerr << "A megadott elem nem szerepel a listaban! Kapott input: \"" + buffer + "\"" << endl; return; }

    cout << "Alapanyag uj neve (elozo eretek megtartasa eseten ures): ";
    std::getline( std::cin, buffer );
    trim( buffer );
    if ( !buffer.empty() )
    {
        if ( pantryList.indexOf( IngredientQ(String(buffer.c_str()), "", 0) ) != -1 )
            cout << "A megadott alapanyag mar szerepel a listaban!" << endl;
        else
            pantryList.get( selected )->setName( String(buffer.c_str()) );
    }

    cout << "Alapanyag uj mertekegysege (elozo eretek megtartasa eseten ures): ";
    std::getline( std::cin, buffer );
    trim( buffer );
    if ( !buffer.empty() ) pantryList.get( selected )->setUnit( String(buffer.c_str()) );

    cout << "Alapanyag uj mennyisege (elozo eretek megtartasa eseten ures): ";
    std::getline( std::cin, buffer );
    trim( buffer );
    if ( !buffer.empty() )
    {
        int new_n;
        try {
            new_n = std::stoi( buffer );
            pantryList.get( selected )->setQuantity( new_n );
        } catch ( std::invalid_argument& ex ) { cerr << "Hibas formatum! Kapott input: \"" + buffer + "\"" << endl; }
    }

    cout << "[Kamra alapanyag sikeresen modositva]" << endl;
}


void Controller::searchByRecipeName() {
    cout << "[Kereses az etel neve alapjan]" << endl;
    if ( recipeList.empty() ) { cout << "A receptlista ures!" << endl; return; }

    cout << "Add meg a keresendo elemet: ";
    std::string buffer;
    std::getline( std::cin, buffer );

    LinkedList< Result<Recipe>* > results = recipeList.search( title_contains( String(buffer.c_str()) ) );
    displaySearchResult( results );
}
void Controller::searchRandom() {
    cout << "[Nincs otletem - veletlenszeru recept]" << endl;
    if ( recipeList.empty() ) { cout << "A receptlista ures!" << endl; return; }

    int selected = 0 + (rand() % static_cast<int>((recipeList.size()-1) - 0 + 1));
    cout << "[Talalat]" << endl << (selected+1) << ". " << recipeList.get(selected)->getTitle() << endl;
}
void Controller::searchByOneIngredient() {
    cout << "[Kereses egy hozzavalo alapjan]" << endl;
    if ( recipeList.empty() ) { cout << "A receptlista ures!" << endl; return; }

    cout << "Add meg a keresendo elemet: ";
    std::string buffer;
    std::getline( std::cin, buffer );

    LinkedList<Ingredient> list = LinkedList<Ingredient>();
    list.push(Ingredient(String(buffer.c_str()), String()));

    LinkedList< Result<Recipe>* > results = recipeList.search( ingredient_contains( list ) );
    displaySearchResult( results );
}
void Controller::serachByMoreIngredient() {
    cout << "[Kereses tobb hozzavalo alapjan]" << endl;
    if ( recipeList.empty() ) { cout << "A receptlista ures!" << endl; return; }

    cout << "Add meg a keresendo elemeket vesszovel elvalasztva: ";
    std::string buffer;
    std::string segment;
    std::getline( std::cin, buffer );
    stringstream ln( buffer );

    LinkedList<Ingredient> list = LinkedList<Ingredient>();
    while ( std::getline( ln, segment, ',' ) )
    {
        std::string tmp = segment;
        if ( !trim( tmp ).empty() ) list.push( Ingredient(String(segment.c_str()), String()) );
    }

    LinkedList< Result<Recipe>* > results = recipeList.search( ingredient_contains( list ) );
    displaySearchResult( results );
}

void Controller::displaySearchResult( LinkedList<Result<Recipe>* >& results ) {
    cout << "[Talalatok]" << endl;
    if ( results.size() < 1 ) { cout << "Nincs talalat." << endl; return; }

    LinkedList< Result<Recipe>* >::Iterator start = results.begin();
    while(start != results.end())
    {
        Result<Recipe>* tmp = *start;
        cout << tmp->getOrder() << ". " << tmp->getPtr()->getTitle() << endl;

        start++;
        delete tmp;
    }
}


// Privát metódusok
bool Controller::modifyIngredientQ(LinkedList<IngredientQ>* list) {
    cout << "1. Uj elem hozzaadasa | 2. Elem torlese | 3. Elem modositasa | 4. Megse\nValassz muveletet: ";
    std::string buffer;
    std::getline( std::cin, buffer );

    int modify;

    try {
        modify = stoi( buffer );
        if ( modify < 1 || modify > 4 ) throw std::out_of_range( "hibas elem" );
    }
    catch ( const std::invalid_argument& ex ) { cerr << "Hibas szam! Kapott input: \"" + buffer + "\"" << endl; return false; }
    catch ( const std::out_of_range& ex ) { cerr << "Nincs ilyen valaszthato lehetoseg! Kapott input: \"" + buffer + "\"" << endl; return false; }

    bool status = false;
    switch ( modify )
    {
        case 1: status = addIngredientQList( list ); break;
        case 2: status = removeIngredientQList( list ); break;
        case 3: status = editIngredientQList( list ); break;
        default: status = false; break;
    }

    return status;
}
bool Controller::modifyStringList(Components::LinkedList<String> *list) {
    cout << "1. Uj elem hozzaadasa | 2. Elem torlese | 3. Elem modositasa | 4. Megse\nValassz muveletet: ";
    std::string buffer;
    std::getline( std::cin, buffer );

    int modify;

    try {
        modify = stoi( buffer );
        if ( modify < 1 || modify > 4 ) throw std::out_of_range( "hibas elem" );
    }
    catch ( const std::invalid_argument& ex ) { cerr << "Hibas szam! Kapott input: \"" + buffer + "\"" << endl; return false; }
    catch ( const std::out_of_range& ex ) { cerr << "Nincs ilyen valaszthato lehetoseg! Kapott input: \"" + buffer + "\"" << endl; return false; }

    bool status = false;
    switch ( modify )
    {
        case 1: status = addStringList( list ); break;
        case 2: status = removeStringList( list ); break;
        case 3: status = editStringList( list ); break;
        default: status = false; break;
    }

    return status;
}

bool Controller::editStringList( Components::LinkedList<String>* list ) {
    if ( list->size() < 1 ) { cerr << "A lista ures!" << endl; return false; }

    cout << "[Lista szerkesztese]" << endl;
    std::string buffer;

    list->printOrderedList( cout );

    cout << "Valaszd ki a modositando elemet: ";
    std::getline( std::cin, buffer );

    int item;
    try {
        item = (std::stoi( buffer ))-1;
        if ( item < 0 || item >= list->size() ) throw std::out_of_range( "hibas elem" );
    }
    catch ( const std::invalid_argument& ex ) { cerr << "Hibas szam! Kapott input: \"" + buffer + "\"" << endl; return false; }
    catch ( const std::out_of_range& ex ) { cerr << "Nem talalhato a megadott elem! Kapott input: \"" + buffer + "\"" << endl; return false; };

    cout << "Add meg az uj elemet: ";
    std::getline( std::cin, buffer );

    std::string tmp = buffer;
    std::string::iterator end_pos = std::remove(tmp.begin(), tmp.end(), ' ');
    tmp.erase(end_pos, tmp.end());
    if ( tmp.empty() ) { cerr << "Hibas formatum! Kapott input: \"" + buffer + "\"" << endl; return false; }

    String* it = list->get(item);
    it->operator=(String(buffer.c_str()));

    return true;
}
bool Controller::addStringList( Components::LinkedList<String>* list ) {
    cout << "[Listaelem hozzaadasa]\nAdd meg az elem nevet: ";
    std::string buffer;
    std::getline( std::cin, buffer );
    std::string tmp = buffer;

    if ( trim( buffer ).empty() ) { cerr << "Hibas formatum! Kapott input: \"" << buffer << "\"" << endl; return false; }
    // if ( list->contains( String(buffer.c_str()) ) ) { cout << "A megadott elem mar szerepel a listaban! Kapott input: \"" + buffer + "\"" << endl; return false; }

    list->push( String( buffer.c_str() ) );
    return true;
}
bool Controller::removeStringList( Components::LinkedList<String>* list ) {
    if ( list->size() < 1 ) { cerr << "A lista ures!" << endl; return false; }

    cout << "[Listaelem torlese]" << endl;

    std::string buffer;

    list->printOrderedList( cout );

    cout << "Valaszd ki a modositando elemet: ";
    std::getline( std::cin, buffer );

    int item;
    try {
        item = (std::stoi( buffer ))-1;
        if ( item < 0 || item >= list->size() ) throw std::out_of_range( "hibas elem" );
    }
    catch ( const std::invalid_argument& ex ) { cerr << "Hibas szam! Kapott input: \"" + buffer + "\"" << endl; return false; }
    catch ( const std::out_of_range& ex ) { cerr << "Nem talalhato a megadott elem! Kapott input: \"" + buffer + "\"" << endl; return false; };

    list->pop( item );
    return true;
}

bool Controller::editIngredientQList( LinkedList<IngredientQ>* list ) {
    if ( list->size() < 1 ) { cerr << "A lista ures!" << endl; return false; }

    cout << "[Listaelem szerkesztese]" << endl;
    list->printOrderedList(cout);

    cout << "Valaszd ki a modositando elemet: ";
    std::string buffer;
    std::getline( std::cin, buffer );

    int item;
    try {
        item = (std::stoi( buffer ))-1;
        if ( item < 0 || item >= list->size() ) throw std::out_of_range( "hibas elem" );
    }
    catch ( const std::invalid_argument& ex ) { cerr << "Hibas szam! Kapott input: \"" + buffer + "\"" << endl; return false; }
    catch ( const std::out_of_range& ex ) { cerr << "Nem talalhato a megadott elem! Kapott input: \"" + buffer + "\"" << endl; return false; };

    IngredientQ* selected = list->get( item );

    cout << "Add meg a modositast az alabbi formatumban (nev mertekegyseg mennyiseg): ";
    std::getline( std::cin, buffer );

    int n_number;
    std::stringstream line( buffer );
    std::vector<std::string> sep;
    std::string segment;
    while ( std::getline( line, segment, ' ' ) )
    {
        sep.push_back( segment );
    }

    try {
        if ( sep.size() != 3 || sep[0].empty() || sep[1].empty() ) throw std::out_of_range( "hibas input" );
        n_number = std::stoi( sep[2] );
    } catch( std::exception& ex ) { cerr << "Hibas formatum! Kapott input: \"" + buffer + "\"" << endl; return false;  }

    if ( list->contains( IngredientQ(String(sep[0].c_str()), String(), 0) ) ) {
        cout << "A megadott elem mar szerepel a listaban! Kapott input: \"" + buffer + "\"" << endl;
        return false;
    }

    selected->setQuantity( n_number );
    selected->setName( String( sep[0].c_str() ) );
    selected->setUnit( String( sep[1].c_str() ) );

    return true;
}
bool Controller::addIngredientQList(Components::LinkedList<Components::IngredientQ> *list) {
    cout << "[Lista szerkesztese]" << endl;
    std::string buffer;

    cout << "Add meg az uj elemet az alabbi formatumban (nev mertekegyseg mennyiseg): ";
    std::getline( std::cin, buffer );

    int n_number;
    std::stringstream line( buffer );
    std::vector<std::string> sep;
    std::string segment;
    while ( std::getline( line, segment, ' ' ) )
    {
        sep.push_back( segment );
    }

    try {
        if ( sep.size() != 3 ) throw std::out_of_range( "hibas input" );
        n_number = std::stoi( sep[2] );
    } catch( std::exception& ex ) { cerr << "Hibas formatum! Kapott input: \"" + buffer + "\"" << endl; return false;  }

    if ( list->contains( IngredientQ(String(sep[0].c_str()), String(), 0) ) ) {
        cout << "A megadott elem mar szerepel a listaban! Kapott input: \"" + buffer + "\"" << endl; return false;
    }

    list->push( IngredientQ( String(sep[0].c_str()), String(sep[1].c_str()), n_number ) );
    return true;
}
bool Controller::removeIngredientQList(Components::LinkedList<Components::IngredientQ> *list) {
    if ( list->size() < 1 ) { cerr << "A lista ures!" << endl; return false; }

    cout << "[Listaelem torlese]" << endl;
    list->printOrderedList(cout);

    cout << "Valaszd ki a torlendo elemet: ";
    std::string buffer;
    std::getline( std::cin, buffer );

    int item;
    try {
        item = (std::stoi( buffer ))-1;
        if ( item < 0 || item >= list->size() ) throw std::out_of_range( "hibas elem" );
    }
    catch ( const std::invalid_argument& ex ) { cerr << "Hibas szam! Kapott input: \"" + buffer + "\"" << endl; return false; }
    catch ( const std::out_of_range& ex ) { cerr << "Nem talalhato a megadott elem! Kapott input: \"" + buffer + "\"" << endl; return false; };

    list->pop( item );
    return true;
}