#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <ctype.h>

#define INPUT_TXT "input.txt"
#define OUTPUT_TXT "output.txt"
#define FIFO_NEV "kimenet_os_beadando_16"

/*

				Bereznai Benjamin CVVJZ4

16. Irjon egy olyan C programot, mely egy fajlból szamparokat kiolvasva meghatarozza a legnagyobb kozos osztojat. A feladat megoldása során hasznaljon nevesitett csovezeteket, valamint a kimenet keruljon egy masik fajlba.

		A kimeneti fajl strukturaja kotott!

		Pelda a bemeneti és kimeneti fájl strukturajara:

		Bemeneti fajl:
		i (Ez jelzi a szamparok darabszamat)
		x y

		Kimeneti fajl(Az x,y jelzi a bemeneti adatokat a z pedig a kimenet eredmenyet):
		x y z
*/

int szamparok_darabszama() { // Fajl megnyitas, ami jelzi, hogy hany db szampar talalhato benne.

    int fd = 0;
    char first_number_char[5];


    fd = open(INPUT_TXT, O_RDONLY);
    if(fd == -1) {
        printf("szamparok_darabszama(): open() hiba!");
        exit(-1);
    }

    read(fd, first_number_char, sizeof(first_number_char));

    close(fd);


    int first_number_int = 0;

    for(int i = 0; isdigit(first_number_char[i]); i++) //Szamok osszefuzese.
        first_number_int = (first_number_int*10) + (first_number_char[i]-'0');

    return first_number_int;

}
void txt_to_char_buf_array(char* txt_buf, int meret) { //Beerkezo input beolvasasa

    int fd, ret;

    fd = open(INPUT_TXT, O_RDONLY);
        if (fd == -1) {
            perror("txt_to_char_buf(char* txt_buf, int meret): open() hiba!");
            exit(-1);
        }

    read(fd,txt_buf,meret);

	close(fd);
}
void char_buf_to_int_array(int* szamok, int szamok_length, char* txt_buf, int txt_buf_length) {

/*  Mukodesi elv:
    A karakter tombon vegig menve, amikor szam,akkor eltaroljuk egy int tombben.
    Ha nem szam pl szokoz, sorteres akkor leptetjuk a tombot.
*/

    int seged_index = 0;

    for(int i = 0; (i < txt_buf_length) && (seged_index < szamok_length); i++) { //a ciklus addig fut amig van karakter tomb, es amig a szamtomb be nem telik
        if( isdigit(txt_buf[i]) ) {//amikor szam a karakter akkor atalakitjuk majd osszefuzzuk

            szamok[seged_index] = (szamok[seged_index]*10)+(txt_buf[i]-'0');//szamok osszefuzese

        } else {

            seged_index++;//amikor nem szam leptetni kell

        }
    }
}
int lnko(int a, int b) {//legnagyobb kozos oszto
    int tmp = 0;

    while(b > 0) {
        tmp = b;
        b = a % b;
        a = tmp;
    }

    return a;

}
void int_feltolt_nullaval(int* tomb, int hossz) {

/*
    tomb nullazo szemet ellen
*/

    for(int i = 0; i < hossz; i++)
        tomb[i] = 0;
}
void char_feltolt_aval(char* tomb, int hossz) {

/*
    tomb nullazo szemet ellen
*/

    for(int i = 0; i < hossz; i++)
        tomb[i] = 'a';
}
void output_txt(char* buf, int length) {//parameterkent megkapott tombot es hosszt beleirja az OUTPUT_TXT-be

    int fd = open(OUTPUT_TXT, O_WRONLY | O_APPEND);
        if (fd == -1) {
            perror("output_txt(): open() hiba!");
            exit(-1);
        }

        write(fd,buf,length);

	close(fd);
}
int main()
{
    int szparok_db = 0;
        szparok_db = szamparok_darabszama();//elso fuggveny, az i-t haratozza meg
        /*Bemeneti fajl:
            i (Ez jelzi a szamparok darabszamat)
            x y*/

    char input_buf[(szparok_db*12+6)];//itt taroljuk el az INPUT.txt tartalmat

    int numbers_length = 0;
        numbers_length = szparok_db*2 + 1;
    int numbers_array[numbers_length];//ebbe alakitom az a karakter tombot
    //////////////////////////////////////////


    //Fel kell tolteni a tombot h elkeruljuk a szemet kiolvasast
    int_feltolt_nullaval(numbers_array, numbers_length);
    char_feltolt_aval(input_buf, sizeof(input_buf));
    //////////////////////////////////////////

    //Beolvasas, konvertalas
    txt_to_char_buf_array(input_buf, sizeof(input_buf));//input.txt ---> char input_buf[]
	char_buf_to_int_array(numbers_array, numbers_length, input_buf, sizeof(input_buf)); //char buf[] ---> int numbers_array[]
    //////////////////////////////////////////


    //Kimenet
    for(int i = 1; i <= numbers_array[0]; i++) {//szamparonkent vegigmenve a tombbon, kihagyva az elso szamot a szamparok szamat adja meg


        int szam_a = numbers_array[i*2-1];
        int szam_b = numbers_array[i*2];
        int szam_lnko = lnko(szam_a, szam_b);
        int szamok_hossza = (floor(log10(abs(szam_a)))+1)+(floor(log10(abs(szam_b)))+1)+(floor(log10(abs(szam_lnko)))+1)+3; //floor(log10(abs(INTEGER_VARIABLA)))+1  ====> a szam karaktereinek darab szama
        char output[szamok_hossza];

        sprintf(output, "%d %d %d\n", szam_a, szam_b, szam_lnko );//beletesszuk a szamparokat es az lnko-t egy karaktertombbe.
        printf("FOR: %s", output);//konzolos kiiratas(ellenorzes)

    //////////////////////////////////////////////////////////////////////////////////////

        unlink(FIFO_NEV);//Torli a csovezeteket

        int fd, ret;
        ret = mkfifo(FIFO_NEV, 00755);//755 jogokkal letrehozzuk a csovet
        if(ret == -1) {
            perror("mkfifo()");
            exit(-1);
        }

        fd = open(FIFO_NEV, O_RDWR);//megnyitjuk a csovet, O_WRONLY-val nem mukodik, mert varja a kiolvasast es addig nem lep tovabb
        if(fd == -1) {
            perror("open() hiba!");
            exit(-1);
        }

        pid_t cpid = fork();//gyerekprocessz letrehozasa
        if (cpid == -1) {
            perror("fork");
            exit(-1);
        }

        write(fd, output, strlen(output));//irom a kimenetet egy csobe

        if (cpid == 0) {    /* gyerek vagyok */
            char buf[szamok_hossza];
            int ret = read(fd, buf, sizeof(buf));//ret megadja hogy mennyit olvasott be a csovezetekbol, igy annyit is iratok ki az OUTPUT_TXT-be
            output_txt(buf, ret);//OUTPUT_TXT-be iras

            exit(0);	// gyerekprocess vege

        } else { /* szulo vagyok */

            wait(NULL);//varok, hogy a gyerek elvegezze a dolgat

            close(fd);//bezarom a veget

            unlink(FIFO_NEV);//letorlom a csovezeteket

        }

    }

    return 0;
}
