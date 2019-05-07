#include "multiproceso.h"

#define BACKLONG 2000

int main (int argc , char** argv){

	int port=5000;
	int val=1;
	int sd, bd,ad,pid,ard,opcion,config;
	char buff[2048];
	char buffer[2048];
	char param[2048];
	int numbytes, cantidad;
	struct sockaddr_in server;
	char * cod_articulo; 
	char * cant_articulo;
	char * token;
	char * resultado;
	char * usuario_id;
	char * archivo_id;
	int enviado;
	char copy[128];
	char name_archivo[50]="";	
	int tuberia_hijos[2];
	int tuberia_padre[2];
	int leido;
	char mensaje[20];
	char * servidor;
	char * user;
       	char * password;
       	char * database;
	//sem_t * semaforo;
	memset(buff,0,sizeof buff);
	memset(mensaje,0,sizeof mensaje);
	memset(param,0,sizeof param);

	/*semaforo= mmap(NULL, sizeof (sem_t),PROT_READ | PROT_WRITE, MAP_SHARED| MAP_ANONYMOUS, -1,0);
	sem_init(semaforo,1,0);*/

	while(1){
		opcion = getopt(argc,argv,"f:");
		switch(opcion){
			case 'f':
				config = open(argv[2],O_RDWR,0644);
				if (config < 0){
					perror("Error al abrir archivo de base de datos.");
                			return -1;
				}
				read(config,param,sizeof param);	
				servidor = strtok(param,"; ");
				user = strtok(NULL,"; ");
				password = strtok(NULL,"; ");
				database = strtok(NULL,"; ");
			break;	
			default:
				servidor = "localhost";
				user = "nfaraz";
				password = "1424/4";
				database = "computacion2";
			break;
		}
	break;
	}

	sd=socket(AF_INET,SOCK_STREAM,0);
	if (sd < 0){
		perror("fallo el socket");
		return -1;
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr= INADDR_ANY;
	bzero(&(server.sin_zero),8);

	setsockopt(sd,SOL_SOCKET, SO_REUSEADDR,(void *)&val,sizeof(val));
	bd=bind (sd,(struct sockaddr*)&server, sizeof (server));
	if (bd < 0 ){
		perror("fallo el bind");
		return -1;	
	}

	listen (sd, BACKLONG);
	signal(SIGCHLD,SIG_IGN);

	while ((ad=accept(sd,NULL,NULL)) > 0){
		if (ad < 0 ){
			perror("fallo el accept");
			return -1;
		}	
		pid = fork ();
		if ( pid == 0 ) {
			printf("Conectado:\n");

			numbytes = read(ad, &buffer, sizeof(buffer));
   			//write(1, buffer, numbytes);
			usuario_id = strtok(buffer,"--> ");
			strcat(name_archivo,usuario_id);
			strcat(name_archivo,".txt");
			
			cantidad = contar(buffer,numbytes) + 1;
			pipe(tuberia_padre);
			pipe(tuberia_hijos);
			
			token = strtok(NULL," ");
			token = strtok(NULL,";");
			for(int i = 0; i<cantidad; i++){
				switch(fork()){	
					case 0:
						if(i != cantidad-1){// hijos procesadores
						//	printf("hijo_procesador\n");
							close(tuberia_hijos[0]);
							strcpy(copy,token);
							cod_articulo = codigo_articulo(token);
							cant_articulo = cantidad_articulo(copy);
							//printf("codigo: %s -cantidad:  %s\n",cod_articulo,cant_articulo);
							resultado = procesamiento(cod_articulo,cant_articulo,servidor,user,password,database);
							write(tuberia_hijos[1],resultado,strlen(resultado));
							close(tuberia_hijos[1]);
							//printf("hijo procesador %d ->desbloqueo padre\n",getpid());
							//sem_post(semaforo);
							return 0;
						}else{// hijo monitor
							//printf("inicio->hijo_monitor\n");
							close(tuberia_padre[0]);
							close(tuberia_hijos[1]);
							remove(name_archivo);
							ard = open(name_archivo, O_CREAT| O_RDWR, 0644);
							if(ard == -1){
                                                		write(tuberia_padre[1],"ERROR!\n",7);
								perror("Error\n");
								return -1;
                                        		}

							while((leido = read(tuberia_hijos[0],buff,sizeof buff))>0){
								write(ard,buff,leido);	
							}

								archivo_id = insertar(usuario_id,servidor,user,password,database);
								write(tuberia_padre[1],archivo_id,sizeof archivo_id);
							/*}else{
								 write(tuberia_padre[1],"Error\n",6);
							}*/
							
							//printf(" hijo monitor %d ->desbloqueo padre\n",getpid());
							//sem_post(semaforo);
							close(ard);
                                                        close(tuberia_hijos[0]);
                                                        close(tuberia_padre[1]);
							return 0 ;
						}
					break;
			
				}
				//printf("bloqueo padre %d \n",getpid());
                              	//sem_wait(semaforo);
				token = strtok(NULL,";");
			}
			//printf("fin->padre\n");
			close(tuberia_padre[1]);
			close(tuberia_hijos[1]);
			close(tuberia_hijos[0]);
			read(tuberia_padre[0],mensaje,sizeof mensaje);
			close(tuberia_padre[0]);

			enviado = enviar(mensaje);
			if(enviado > 0){
				printf("enviado\n");
			}else{
				printf("error");
			}
			return 0;		
		}
		close (ad);
	}
	return 0;
}
