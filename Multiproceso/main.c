#include "multiproceso.h"

#define sms(a,b)(a == 1) ? b : (void)0

#define BACKLONG 2000


int main (int argc , char** argv){

	int port=5000;
	int val=1,a;
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

	while (1){
		opcion = getopt (argc, argv, "f:v");
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
				a=1;
			break;
			case 'v':
				a = 1;
				servidor = "localhost";
                                user = "nfaraz";
                                password = "1424/4";
                                database = "computacion2";
	
			break;
			default:
				a = 0;
				servidor = "localhost";
                                user = "nfaraz";
                                password = "1424/4";
                                database = "computacion2";
			break;
		}
		break;
	}

	sms(a,printf("server:%s \n",servidor));
	sms(a,printf("usuario:%s \n",user));
	sms(a,printf("database:%s \n",database));

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
			sms(a,printf("Conectado:\n"));
			sms(a,printf("Padre %d\n",getpid()));
			numbytes = read(ad, &buffer, sizeof(buffer));
			sms(a,printf("%s\n",buffer));
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
							close(tuberia_hijos[0]);
							strcpy(copy,token);
							cod_articulo = codigo_articulo(token);
							cant_articulo = cantidad_articulo(copy);
							sms(a,printf("Hijo %d --> codigo: %s cantidad:  %s\n",getpid(),cod_articulo,cant_articulo));
							resultado = procesamiento(cod_articulo,cant_articulo,servidor,user,password,database);
							write(tuberia_hijos[1],resultado,strlen(resultado));
							close(tuberia_hijos[1]);
							//sem_post(semaforo);
							return 0;
						}else{// hijo monitor
							sms(a,printf("Hijo monitor %d \n",getpid()));
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
								sms(a,write(1,buff,leido));	
							}
							
							archivo_id = insertar(usuario_id,servidor,user,password,database);
							write(tuberia_padre[1],archivo_id,sizeof archivo_id);
							sms(a,printf("Archivo_id --> %s\n",archivo_id));
								
							//sem_post(semaforo);
							close(ard);
                                                        close(tuberia_hijos[0]);
                                                        close(tuberia_padre[1]);
							return 0 ;
						}
					break;
			
				}
                              	//sem_wait(semaforo);
				token = strtok(NULL,";");
			}
			close(tuberia_padre[1]);
			close(tuberia_hijos[1]);
			close(tuberia_hijos[0]);
			read(tuberia_padre[0],mensaje,sizeof mensaje);
			close(tuberia_padre[0]);

			enviado = enviar(mensaje);
			if(enviado > 0){
				sms(a,printf("enviado\n"));
			}else{
				sms(a,printf("error\n"));
			}
			return 0;		
		}
		close (ad);
	}
	return 0;
}
