#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <inttypes.h>

#include "DBGpthread.h"
#include "printerror.h"

#define CANI 5
#define MAX_CANI 4

pthread_mutex_t mutexTicket;
pthread_mutex_t mutexTurno;

pthread_cond_t condTurno;
pthread_cond_t condCalcioDiVic;
pthread_cond_t condVicPedala;

int caniCheMordono = 0;
int distributoreTicket = 0;
int turnoCorrente = 0;

void *cane( void *arg ) {
    char Clabel[128];
    int mioTurno;
    intptr_t indice = ( intptr_t )arg;

    while( 1 ) {
        DBGpthread_mutex_lock( &mutexTicket, "cane prende ticket\n" );
        mioTurno = distributoreTicket++;
        DBGpthread_mutex_unlock( &mutexTicket, "cane si mette in coda\n" );
        sprintf( Clabel, "Cane %" PRIiPTR " con turno %d", indice, mioTurno );
        DBGpthread_mutex_lock( &mutexTurno, Clabel );
        while( caniCheMordono == MAX_CANI || mioTurno != turnoCorrente ) {
            printf( "%s: in ATTESA, caviglie piene o non il suo turno (turno corrente = %d, caviglie morse = %d)\n", Clabel, turnoCorrente, caniCheMordono );
            DBGpthread_cond_wait( &condTurno, &mutexTurno, Clabel );
        }
        turnoCorrente++;
        printf( "%s: INIZIA a mordere, ATTENDE calcio di Vic\n", Clabel );
        caniCheMordono++;
        if( caniCheMordono == 1 ) {
            printf( "%s: e\' il PRIMO: sveglia Vic\n", Clabel );
            DBGpthread_cond_signal( &condVicPedala, Clabel );
        }
        DBGpthread_cond_wait( &condCalcioDiVic, &mutexTurno, Clabel );
        printf( "%s: AHIAAA, calcio ricevuto\n", Clabel );
        DBGpthread_cond_broadcast( &condTurno, Clabel );
        DBGpthread_mutex_unlock( &mutexTurno, Clabel );
    }
    pthread_exit( NULL );
}

void *vic( void *arg ) {

    char Vlabel[128];
    sprintf( Vlabel, "Vic" );
    while( 1 ) {
        DBGpthread_mutex_lock( &mutexTurno, Vlabel );
        while( caniCheMordono <= 0 ) {
            printf( "%s: infarto in arrivo (pedala)\n", Vlabel );
            DBGpthread_cond_wait( &condVicPedala, &mutexTurno, Vlabel );
        }
        printf( "%s: DIO $@#! che male!!! vai via bastardo!\n", Vlabel );
        caniCheMordono--;
        DBGpthread_cond_signal( &condCalcioDiVic, Vlabel );
        DBGpthread_mutex_unlock( &mutexTurno, Vlabel );
        sleep( 1 );
    }
    pthread_exit( NULL );
}

int main( void ) {

    int rc;
    intptr_t i;
    pthread_t th;

    DBGpthread_mutex_init( &mutexTicket, NULL, "main" );
    DBGpthread_mutex_init( &mutexTurno, NULL, "main" );
    DBGpthread_cond_init( &condCalcioDiVic, NULL, "main" );
    DBGpthread_cond_init( &condVicPedala, NULL, "main" );
    DBGpthread_cond_init( &condTurno, NULL, "main" );

    rc = pthread_create( &th, NULL, vic, NULL );
    if( rc ) {
        PrintERROR_andExit( rc, "Creazione vic" );
    }

    for( i=0; i<CANI; i++ ){
        rc = pthread_create( &th, NULL, cane, ( void *)i );
        if( rc ) {
            PrintERROR_andExit( rc, "Creazione vic" );
        }
    }
    pthread_exit( NULL );
    return 0;
}

