#include <stdbool.h>
#include <stdint.h>

//----------------------------definicja zadan---------------------------------
#define max_zadan 7
#define max_podzadan 7

uint8_t index_zadan;
uint8_t index_podzanan;

uint8_t aktualne_zadanie;
uint8_t aktualne_podzadanie;
uint8_t Tab_zadan[max_zadan][max_podzadan];


struct {
   uint8_t PP_Vol; //Zadania pomiaru napiecia ze slotow w kolejnosci 1,2,3,4,5,6,7
   uint8_t PP_Epro; //Zadania spr pamieci ze slotow w kolejnosci 1,2,3,4,5,6,7
   uint8_t PO_Vol; //Zadania wykonywania pomiarow przez ADC na testerze
   uint8_t BB_B;   //Zadania Baterii 1-napiecie
   uint8_t MM_R;   //Czytanie rejestrow z plytki telcor przez modbus
   uint8_t MM_W;   //Zapisywanie rejestrow z plytki telcor przez modbus
   uint8_t TT_G;   //Zadanie testu sprzetu
} flaga_zadan;

