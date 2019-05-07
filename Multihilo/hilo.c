#include "multihilo.h"

pthread_mutex_t sem;

void *hilo (void* sd_conn){
	
	pthread_mutex_lock(&sem);
	int sdc, leido;
	char buffer[2048];
	char * pedido;
	char * respuesta;
	char * control;
	int sid;
	memset(buffer,0,sizeof buffer);

        sdc = *((int *)sd_conn);
        free(sd_conn);
	
	leido = read(sdc,buffer,sizeof buffer);
	write (1,buffer,leido);
	write (1,"\n",1);
	
	control = strtok(buffer,":/ ");
	
	if(!strcmp(control,"0")){
        pedido = strtok(NULL,"/");
        sid = enviar(pedido);
        	if(sid < 0 ){
                	close(sdc);
                	pthread_mutex_unlock(&sem);
                	pthread_exit(NULL);
        	}
	}else if(!strcmp(control,"1")){
	       	respuesta = strtok(NULL,"/");
	       	sid = responder(respuesta);
                if(sid < 0 ){
                        close(sdc);
                        pthread_mutex_unlock(&sem);
                        pthread_exit(NULL);
                }
	
	}else{		
		printf("Error\n");
		close(sdc);
        	pthread_mutex_unlock(&sem);
        	pthread_exit(NULL);
	}
	
	close(sdc); 
	pthread_mutex_unlock(&sem);
	pthread_exit(NULL);
}

