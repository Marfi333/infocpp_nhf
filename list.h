#ifndef NHF4_LIST_H
#define NHF4_LIST_H
/**
 * \file list.h
 *
 * Ez a fájl tartalmazza a működéshez szükséges Result, és LinkedList osztályokat
 */

#include <cstddef>
#include "memtrace.h"
#include "string5.h"


namespace Components
{
    /**
     * Result osztály
     * A keresési eredmények tárolásához szükséges
     * Tárolja a találat eredeti helyét az eredeti listában, és
     * a találatra mutató pointert
     */
    template<class T>
    class Result
    {
    private:
        int order;  /// A találat eredeti helye a listában
        T* ptr;     /// A találatra mutató pointer

    public:
        /// Default konstruktor
        /// Inicializáljuk a találatot és a pointert
        /// @param p - Találatra mutató pointer
        /// @param o - Sorszám
        explicit Result( T* p, int o ) :ptr(p), order(o) {};

        /// Pointer getter
        /// @return találatra mutató pointer
        T* getPtr() { return ptr; }

        /// Sorszám getter
        /// @return találat sorszáma
        int getOrder() { return order; }
    };

    /**
     * LinkedList osztály
     * A program működéséhez szükséges legfontosabb osztály
     * Láncolva tárolja a megadott típusú adatokat, és megvalósítja a fontosabb
     * műveletekhez szükséges metódusokat
     */
    template<class T>
    class LinkedList
    {
        /**
         * Node osztály
         * Ebben tároljuk az adatokat, és ezt láncoljuk össze
         */
        class Node
        {
        public:
            T item;     /// Adat
            Node* next; /// Következő elemre mutató pointer

            /// Dafault konstruktor
            /// Inicializáljuk a következő elemre mutató pointert null-lal
            Node() :next( nullptr ) {};
        };

    private:
        Node* start;    /// A legelső elemre mutató pointer (strázsa)
        Node* back;     /// A legutolsó elemre mutató pointer (strázsa)
        size_t siz;     /// A lista hossza

        /// Indexelő operátor
        /// Biztonság kedvéért privát, hogy ne legyen összekeverhető egy tömbbel
        /// Ha az elem nem szerepel a listában std::out_of_range hibát dob
        /// @param index - a keresett index
        /// @return Tárolt elem referenciája
        T& operator[]( int index );
    public:
        /// Default konstruktor
        /// Inicializáljuk a kezdő,vég strázsát, és a lista hosszát
        LinkedList() : start(nullptr), back(nullptr), siz(0) {};

        /// Iterátor osztály elődeklarálása
        class Iterator;

        /// Iterátor, ami a lista legelső elemére mutat
        /// @return Iterátor az első elemre
        Iterator begin() { return Iterator( *this ); };

        /// Iterátor ami a lista legutolsó utáni elemére mutat
        /// @return Iterátor az utolsó utáni elemre
        Iterator end() { return Iterator(); };

        /// Lista hosszának gettere
        /// @return int - a lista hossza
        int size() const { return siz; }

        /// Üres-e a lista
        /// @return bool - üres-e a lista
        bool empty() const { return siz == 0; }

        /// Hozzáadja a paraméterben kapott elemet a listához
        /// @param data - az elem referenciája
        /// @return int - az elem indexe
        int push( const T& data );

        /// Kiveszi a listából a megadott elemet
        /// @param index - az elem indexe a listában
        void pop( int index );

        /// Törli és felszabadítja az összes elemet a listából
        void clear();

        /// Megadja hogy a keresett elem szerepel-e a listában
        /// @param element - a keresett elem
        bool contains( const T* element );
        bool contains( T element );

        /// Visszaadja a keresett elemre mutató pointert
        /// Az indexelő operátort valósítja meg, egy listásabb formátumban
        /// std::out_of_range hibát dob ha nem szerepel a listában
        /// @param index - a keresett elem indexe
        /// @return T* - az elemre mutató pointer
        T* get( int index ) { return &operator[](index); };

        /// Megadja a keresett elem indexét a listában
        /// -1-gyel tér vissza ha az elem nincs a listában
        /// @param item - A keresett elem
        /// @return int - a talált elem indexe
        int indexOf( const T* item );
        int indexOf( const T item );

        /// Egy számozott listát ír ki a kapott kimenetre a tárolt elemekkel
        /// @param ostream - standard kimenet
        /// @param displayEmpty - kiírja-e a kimenetre ha a lista üres? default = false
        /// @param from - szám, ahonnan az indexelést kezdje. default = 1
        void printOrderedList( std::ostream& ostream, bool displayEmpty = false, int from = 1 );

        /// Generikus keresés a listában
        /// Létrehoz egy Result elemeket tároló listát, amibe azoknak az elemeknek a
        /// pointereit helyezi, melyeknél a paraméterben kapott függvény igazra értékelődik
        /// @param func - funktor, ami igaz/hamis-sal tér vissza, és megvalósít egy keresési feltételt
        /// @return LinkedList<Result<T>* > - eredményeket tartalmazó láncolt lista
        template<class Func>
        LinkedList<Result<T>* > search( Func func )
        {
            LinkedList<Result<T>* > ret = LinkedList<Result<T>* >();

            Iterator start = begin();
            for(int i = 1; start != end(); start++, i++)
            {
                if ( func(*start) ) ret.push( new Result<T>( &*start, i ) );
            }

            return ret;
        }

        /// Destruktor
        /// Felszabadítja a listát
        ~LinkedList();


        /**
         * Iterator osztály
         * A lista loop-olását segítő osztály
         */
        class Iterator
        {
        private:
            Node* current; /// Az aktuális elemre mutató pointer

        public:
            /// Konstruktor, ami inicializálja az utolsó utáni iterátort
            Iterator() :current( nullptr ) {};

            /// Konstruktor, ami inicializálja az elsp iterátort
            /// @param list - lista amin iterálni szeretnénk
            Iterator( const LinkedList<T>& list ) :current( list.start ) {};

            /// ++ operátorok
            /// Növeli az iterátor értékét (ugrás a következő elemre)
            /// @return Iterator& - a következő iterátor referenciája
            Iterator& operator++();
            const Iterator operator++( int );

            /// != operátor
            /// Megadja hogy az összehasonlított iterátorok megegyeznek-e
            /// @param i - összehasonlítandó iterátor (kifejezés jobb oldala)
            /// @return bool - megegyeznek-e az iterátorok
            bool operator!=( const Iterator &i ) const;

            /// & operátor
            /// Visszaadja az aktuális elem referenciáját
            /// @return T& - aktuális elem referenciája
            T& operator*();

            /// * operátor
            /// Visszaadja az aktuális elemre mutató pointert
            /// @return T* - aktuális elem pointere
            T* operator->();
        };
    };

    /// Függvények megvalósítása
    /// Mivel template osztályok, ezért a header fájlban kell megírni őket

    template<class T>
    void LinkedList<T>::clear() {
        Node* current = start;
        while ( current != nullptr )
        {
            Node* next = current->next;
            delete current;
            current = next;
        }

        start = nullptr;
        back = nullptr;
        siz = 0;
    }

    template<class T>
    int LinkedList<T>::push(const T &data) {
        Node* tmp = new Node();
        tmp->item = data;

        siz++;

        if ( start == nullptr )
        {
            start = tmp;
            back = tmp;
            return siz - 1;
        }

        back->next = tmp;
        back = back->next;

        return siz - 1;
    }

    template<class T>
    T &LinkedList<T>::operator[](int index) {
        if ( index < 0 ) throw std::out_of_range("Bad indexing");

        Iterator curr = Iterator(*this);
        Iterator end = Iterator();
        int counter = 0;

        while ( curr != end )
        {
            if ( counter == index ) return *curr;
            curr++; counter++;
        }

        throw std::out_of_range("Bad indexing");
    }

    template<class T>
    void LinkedList<T>::pop(int index) {
        if ( index < 0 || index >= size() ) throw std::out_of_range("Bad indexing");

        if ( index == 0 )
        {
            Node* tmp = start;
            start = start->next;

            delete tmp;
            siz--;
            return;
        }

        Node* current = start->next;
        Node* prev = start;
        int counter = 1;

        while ( current != nullptr )
        {
            if ( counter == index )
            {
                if ( counter == size()-1 )
                {
                    prev->next = nullptr;
                    back = prev;
                    delete current;
                }
                else
                {
                    prev->next = current->next;
                    delete current;
                }
                siz--;
                return;
            }

            prev = current;
            current = current->next;
            counter++;
        }
    }

    template<class T>
    bool LinkedList<T>::contains( const T* element ) {
        return indexOf( element ) != -1;
    }

    template<class T>
    bool LinkedList<T>::contains( T element ) {
        return indexOf( element ) != -1;
    }

    template<class T>
    int LinkedList<T>::indexOf( const T* element) {
        for ( int i = 0; i < size(); i++ )
        {
            if ( *get(i) == *element )
            {
                return i;
            }
        }

        return -1;
    }

    template<class T>
    int LinkedList<T>::indexOf( const T element) {
        for ( int i = 0; i < size(); i++ )
        {
            if ( *get(i) == element )
            {
                return i;
            }
        }

        return -1;
    }

    template<class T>
    void LinkedList<T>::printOrderedList(std::ostream &ostream, bool displayEmpty, int from) {
        LinkedList<T>::Iterator start = begin();
        for ( ; start != end(); start++ , from++ )
        {
            ostream << from << ". ";
            (*start).printDetails( ostream );
            ostream << std::endl;
        }

        if ( displayEmpty && size() < 1 )
        {
            ostream << "A lista ures." << std::endl;
        }
    }

    template<class T>
    LinkedList<T>::~LinkedList() {
        Node* current = start;
        while ( current != nullptr )
        {
            Node* next = current->next;
            delete current;
            current = next;
        }

        start = nullptr;
        siz = 0;
    }

    template<typename T>
    typename LinkedList<T>::Iterator& LinkedList<T>::Iterator::operator++() {
        if ( current != nullptr )
        {
            current = current->next;
            //if ( current->next == nullptr ) current = nullptr;
        }
        return (*this);
    }

    template<typename T>
    const typename LinkedList<T>::Iterator LinkedList<T>::Iterator::operator++(int) {
        LinkedList<T>::Iterator tmp = *this;
        operator++();
        return (tmp);
    }

    template<class T>
    bool LinkedList<T>::Iterator::operator!=(const LinkedList<T>::Iterator &i) const {
        return ( current != i.current );
    }

    template<class T>
    T &LinkedList<T>::Iterator::operator*() {
        if ( current != nullptr ) return current->item;
        else throw std::out_of_range( "Accessed item is null" );
    }

    template<class T>
    T *LinkedList<T>::Iterator::operator->() {
        if ( current != nullptr ) return (&current->item);
        else throw std::out_of_range( "Accessed item is null" );
    }
}

#endif // NHF4_LIST_H