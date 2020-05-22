#include "proyecto.h"
#include "ui_proyecto.h"
#include <string.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/wait.h>

//definiciones
#define VOL_NOM 120
#define FRE_NOM 60
#define TEMP_NOM 40
#define VEL_NOM 1800


using namespace std;
proyecto::proyecto(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::proyecto)
{
    ui->setupUi(this);
    ui->msgerror->close();
    setWindowTitle("laboratorio final de alse");// se le pone el nombre del proyecto en la ventana
    QStringList titulos;
    ui->tableWidget->setColumnCount(7);//declaramos cuantas columnas queremos en nuesta tablewidget
    titulos << "CARNÉ" << "VELOCIDAD" << "FRECUENCIA" << "VOLTAJE" << "TEMPERATURA" << "TIEMPO OP" << "VERIFICACION";//asignamos los nombres de cada columna de izquierda a derecha.
    ui->tableWidget->setHorizontalHeaderLabels(titulos);//le pasamos los nombres de las columnas a la tablewidget
}

proyecto::~proyecto()
{
    delete ui;
}


void proyecto::on_pushButton_clicked()
{
    //definicion de variables generales
    long int numerocar;//numero del carnet en enterosingresado por el usuario
    int pid,vel=VEL_NOM,frec=FRE_NOM,volt=VOL_NOM,temp=TEMP_NOM;
    double tv,tfr=FRE_NOM*0.05,tvolt,tt;//tolerancias tipo entero
    char *argv[3],dir_c[16]={"./UDPCLIENTE.c"};
    char carn_id[10]={0};//numero de carnet como tipo char
    double car,velo,fre,vol,tem,top;//variables para guardar los datos recibidos
    char tv1[3],tt1[2],tvol1[2];//asignacion variables de tolerancia tipo char
    int cont=0;

    QString num;

    num=ui->ncarnet->text();
    numerocar=num.toInt();//pasar el dato ingresado por el usuario a long

 //pasar de numero a string el numero del carné para ser enviado como argumento
    for(int i=0;i<8;i++)
    {
        std::string cStd = num.toStdString();
        carn_id[i]=cStd[i];
    }
    if(numerocar>=2000000 && numerocar<=2400000){
//desaparecer los qlabel que no necesitamos________________________________________
    ui->mdinfo->close();
    ui->msgerror->close();
    ui->esvelr->close();
    ui->esvelr_2->close();
    ui->esvolr->close();
    ui->esvolr_2->close();
    ui->esfrecr->close();
    ui->esfrecr_2->close();
    ui->estempr->close();
    ui->estempr_2->close();


//inicio proceso (hijo-padre)____________________________________________________
    pid = fork();

    switch (pid)
    {
            case 0: //proceso hijo

                 pid_t pid_hijo;
                 argv[0]=dir_c;
                 argv[1]=carn_id;
                 argv[2]= NULL;


                 execv("/home/alse/proyecto1/UDPCLIENTE.bin",argv);
            break;

            default://entra al proceso padre

            pid_hijo=wait(NULL);

            ifstream infile("/home/alse/proyecto1/datos_motor.txt");
//asignacion de variables actuales___________________________________________
            infile >> car >> velo >> fre >> vol >> tem >> top;


//visualizar datos actuales del motor
            ui->mvel->setText(QString::number(velo));
            ui->lcdfrec->display(fre);
            ui->lcdvol->display(vol);
            ui->lcdtemp->display(tem);
            //ui->carnetconf->setText();
 //calculo tolerancias_________________________________________-

            //tolerancia de velocidad
            tv1[0]=carn_id[1];
            tv1[1]=carn_id[2];
            tv = atof(tv1);
            tv=1800*(tv/100);
            ui->tolvel->setText(QString::number(tv));
            ui->VEL_NO->setText(QString::number(vel));

            //tolerancia de temperatura
            tt1[0]=carn_id[6];
            tt = atof(tt1);
            tt=40*(tt/100);
            ui->toltem->setText(QString::number(tt));
            ui->TEMP_NO->setText(QString::number(temp));

            //tolerancia de voltaje
            tvol1[0]=carn_id[4];
            tvolt = atof(tvol1);
            tvolt=120*(tvolt/100);
            ui->tolvol->setText(QString::number(tvolt));
            ui->VOL_NO->setText(QString::number(volt));

            //tolerancia de frecuencia
            ui->tolfrec->setText(QString::number(tfr));
            ui->FRE_NO->setText(QString::number(frec));


 //condicion de estado_________________________________________________________

            //condicion de estado de la velocidad
            if(velo <= (tv+vel) && velo >= (vel-tv)){
                ui->esvelr->close();
                ui->esvelr_2->show();

            }else{
                ui->esvelr->show();
                ui->esvelr_2->close();
            }

            //condicion de estado de la voltaje
            if(vol <= (tvolt+volt) && vol >= (volt-tvolt)){
                ui->esvolr->close();
                ui->esvolr_2->show();

            }else{
                ui->esvolr->show();
                ui->esvolr_2->close();
            }

            //condicion de estado de la frecuencia
            if(fre <= (tfr+frec) && fre >= (frec -tfr)){
                ui->esfrecr->close();
                ui->esfrecr_2->show();

            }else{
                ui->esfrecr->show();
                ui->esfrecr_2->close();
            }

            //condicion de estado de la tempetaruta
            if(tem <= (tt+temp) && tem >= (temp-tt)){
                ui->estempr->close();
                ui->estempr_2->show();

            }else{
                ui->estempr->show();
                ui->estempr_2->close();
            }

//TIEMPO DE OPERACION______________________________________________________________________________________________________________

            //TIEMPO ACTUAL
            long TIEMPO;
            TIEMPO=chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();

            //TIEMPO DE OPERACION
            TIEMPO=(TIEMPO-top);

            // en esta parte de codigo se convierte el tiempo timestamp
            time_t rawtime=TIEMPO/1000;
            struct tm ts;
            char dato[120];
            ts=*localtime(&rawtime);
            strftime(dato, sizeof(dato),"%d %H:%M:%S %Z",&ts);


//guardar datos de imprecion_____________________________________________________________________________-

             ofstream file;
             file.open("/home/alse/proyecto1/guardados.txt",std::ios_base::app);
             file << "carné:" << numerocar << " velocidad:" << velo << " frecuencia:" << fre <<" voltaje:" << vol << " temperatura:" << tem << " tiempo op:" << dato;
             file << endl;
             file.close();


//REGISTRO DE DATOS EN LA TABLA_________________________________________________________________________________________________________________
            int fila;
            fila = ui->tableWidget->rowCount() - 1;
            ui->tableWidget->insertRow(ui->tableWidget->rowCount());
            ui->tableWidget->setItem(fila,0,new QTableWidgetItem(QString::number(numerocar)));// asignamos contenido a cada columna resptivamente de izquierda a derecha empezando desde 0 hasta n columnas.
            ui->tableWidget->setItem(fila,1,new QTableWidgetItem(QString::number(velo)));
            ui->tableWidget->setItem(fila,2,new QTableWidgetItem(QString::number(fre)));
            ui->tableWidget->setItem(fila,3,new QTableWidgetItem(QString::number(vol)));
            ui->tableWidget->setItem(fila,4,new QTableWidgetItem(QString::number(tem)));
            //ui->tableWidget->setItem(fila,5,new QTableWidgetItem(QString::number(top)));
            ui->tableWidget->setItem(fila,5,new QTableWidgetItem(dato));



            cont++;
                        if(car!=numerocar  && cont==1){

                            ui->mdinfo->show();
                            ui->msgerror->show();
                            cont=0;
                            ui->tableWidget->setItem(fila,6,new QTableWidgetItem("INCORRECTO"));
                        }
                        else{
                            ui->mdinfo->close();
                            ui->msgerror->close();
                            ui->tableWidget->setItem(fila,6,new QTableWidgetItem("CORRECTO"));
                        }
    }

    }
    else {
      ui->mdinfo->show();
      ui->msgerror->show();

    }



}

