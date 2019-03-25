#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <netdb.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define MAX_CACHE_SIZE 5<<20 /* 5MB */
#define MAX_OBJECT_SIZE 512<<10 /* 512KB */
#define REQUEST_SIZE 1<<10 /* 1KB */

void parse_and_response(void*);
void error(char*);
void cleanExit(){printf("clean exiting...\n");exit(0);}
void logging(char*, char*, char*, int);

/* thread실행 시 필요한 arguments들을 모아둔 struct */
typedef struct arguments_for_thread{
	int fd;
	char ip[100];
} fd_ip;

/* cache 자료 저장 linkedlist */
typedef struct _cache_linkedList{
	char *data;
	int size;
	char *uri;
	struct _cache_linkedList *next;
} cache;
cache *head = NULL;
int cache_size = 0;
cache* find_in_cache(char*);
void add_cache(char*, int, char*);


/* 쓰레드간 동기화를 위한 lock */
pthread_rwlock_t cacheLock;
pthread_rwlock_t logLock;
void rdLock(pthread_rwlock_t*);
void wrLock(pthread_rwlock_t*);
void unLock(pthread_rwlock_t*);
int main(int argc, char *argv[]){

	if(argc < 2){
		printf("./proxy PORT");
		exit(0);
	}
	int serverfd, clientfd;
	pthread_t tid;
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_len;
	char request[9999];
	int port = atoi(argv[1]);
	pthread_rwlock_init(&cacheLock,NULL);
	pthread_rwlock_init(&logLock,NULL);

	signal(SIGTERM, cleanExit);
	signal(SIGINT, cleanExit);
	signal(SIGPIPE, SIG_IGN);
	
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	//Internet, TCP socket
	if((serverfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) error("socket error");
	
	if((bind(serverfd, (struct sockaddr *)&server_addr
	,sizeof(server_addr))) < 0) error("bind error");
	
	if((listen(serverfd, 10)) < 0) error("listen error");
	
	//accept
	while(1){
	
	client_len = sizeof(client_addr);
	if((clientfd = accept(serverfd, (struct sockaddr *)&client_addr
	,&client_len)) < 0 ) error("accept error");
	
	
	//arguments passing to thread.
	fd_ip args;
	args.fd = clientfd;
	strcpy(args.ip,inet_ntoa(client_addr.sin_addr));

	//run thread
	pthread_create(&tid, NULL, (void*)parse_and_response, (void *)&args);
	}
	
	/* Never come */
	close(serverfd);
	pthread_rwlock_destroy(&cacheLock);
	pthread_rwlock_destroy(&logLock);
	return 0;
}

/* parse_and_response 
   clientfd와 ip를 가진 struct 를 인자로 받습니다.
   ip는 logging함수의 인자로 사용됩니다.

   client로 부터 받은 request를 parsing한후
   실제 서버로부터 데이터를 받아 그대로 전달해 줍니다. */
void parse_and_response(void *args){
	pthread_detach(pthread_self());
	char client_request[REQUEST_SIZE], proxy_request[REQUEST_SIZE], buf[REQUEST_SIZE], date[1000], uri[1000];
	char *token_by_line, *host;
	cache *hit = NULL;
	int size = 0;
	int n = 0;
	fd_ip *ret = (fd_ip *)args;
	int clientfd = ret->fd;
	memset(&client_request, 0, REQUEST_SIZE);
	memset(&proxy_request, 0, REQUEST_SIZE);

	if((n = recv(clientfd, &client_request, sizeof(client_request),0)) < 0) {
		error("recv error");
		close(clientfd);
		printf("unexpected error occured\n");
		pthread_exit(NULL);
	}
	if(n == 0) {
		close(clientfd); 
		pthread_exit(NULL);
	}
	//Parsing and make proxy_request
	memset(&buf, 0, REQUEST_SIZE);

	strcpy(buf,client_request);
	token_by_line = strtok(buf,"\r\n");
	
	
	char *t = strtok(buf, " ");
	
	if(strcmp(t, "GET") == 0){
		t = strtok(NULL, " ");
		sprintf(proxy_request, "GET %s HTTP/1.0\r\n",t);	
	}else{
		close(clientfd);
		pthread_exit(NULL);
	}
	strcpy(uri,t);
	
	printf("uri : %s \n",uri);
	
	rdLock(&cacheLock);
	if((hit = find_in_cache(uri)) != NULL){
		unLock(&cacheLock);
		if((send(clientfd,hit->data , hit->size, 0)) < 0) error("send error");
		close(clientfd);
		pthread_exit(NULL);
	}else {
		unLock(&cacheLock);
		printf("cache missed\n");
	}
		
	token_by_line = strtok(client_request, "\r\n");	

	while((token_by_line = strtok(NULL, "\r\n")) != NULL){

		if(strstr(token_by_line, "Proxy-Connection:") != NULL){ // REMOVE
		}
		else if(strstr(token_by_line, "Connection:") != NULL){ // REMOVE
	
		}
		else if(strstr(token_by_line, "Host:") != NULL){
			sprintf(proxy_request,"%s%s\r\n",proxy_request,token_by_line);
			host = token_by_line + strlen("Host: ");
		}
		else{
			sprintf(proxy_request,"%s%s\r\n",proxy_request,token_by_line);
		}	
	}
	sprintf(proxy_request, "%s\n",proxy_request);

	/* proxy_server가 end_server와 연결합니다.
	   받은 데이터를 client에게 그대로 전달 합니다.*/
	struct sockaddr_in end_server_addr;
	struct hostent *end_server;
	int endserverfd;
	char data_from_endserver[MAX_OBJECT_SIZE];
	int portno = 80;
	
	if((endserverfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) error("socket error");

	/* 만약 포트번호가 명시되어 있으면, portno를 바꿉니다. */
	if(strtok(host, ":") != NULL){
		if(strtok(NULL, ":") != NULL){
		portno = atoi(host+strlen(host)+1);
		}
	}

	printf("connecting... domain : %s, portno : %d\n",host,portno);
	if((end_server = gethostbyname(host)) == NULL ) error("gethostbyname error");
	memset(&end_server_addr, 0, sizeof(end_server_addr));
	end_server_addr.sin_family = AF_INET;
	memcpy(&end_server_addr.sin_addr.s_addr, end_server->h_addr,end_server->h_length);
	end_server_addr.sin_port = htons(portno);
	if(connect(endserverfd, (struct sockaddr *)&end_server_addr, sizeof(end_server_addr)) < 0) error("connect error");
	if((n = send(endserverfd, proxy_request, strlen(proxy_request), 0)) < 0) error("send error");

	//receive data from endserver & send data to client by MAX_OBJECT_SIZE
	int j = 0;
	char *p, *q;
	while((n = recv(endserverfd, data_from_endserver, MAX_OBJECT_SIZE,0)) > 0){
		if((send(clientfd, data_from_endserver, n, 0)) < 0) error("send error");
		memcpy(buf,data_from_endserver,n);
		//get date from data for logging
		p = strtok(buf, "\r\n");
		while((p = strtok(NULL, "\r\n")) != NULL){
			if((q = strstr(p, "Date: ")) != NULL){
				strcpy(date, q+6);
			}
		}
		j++;
		size += n;
	}
	if(n < 0) error("recv error");

	/* 받은 데이터가 MAX_OBJECT_SIZE 보다 작다면, Cache 에 저장 합니다. */
	if(j == 1){ 
		wrLock(&cacheLock);
		add_cache(data_from_endserver, size, uri);
		unLock(&cacheLock);
	}
	
	wrLock(&logLock);
	logging(date, ret->ip, host, size);
	unLock(&logLock);

	close(endserverfd);
	close(clientfd);
	pthread_exit(NULL);
	}

/* logging : proxy.log라는 파일을 만들거나, 열어서 
   날짜, ip, host, 크기를 기록 합니다. */
void logging(char *date,char *ip, char *host, int size){
	printf("\t###logging###\n");
	char buf[512];
	int fd;
	
	printf("date : %s host : %s\n",date,host);
	sprintf(buf,"%s: %s %s %d\n",date, ip, host, size);
	if((fd = open("proxy.log", O_WRONLY | O_CREAT)) < 0) error("open error");
	if(lseek(fd, 0, SEEK_END) < 0) error("lseek error");	
	if(write(fd, buf, strlen(buf)) < 0) error("write error");

	close(fd);
	return;
	}
/* add_cache : cache linkedList에 받은 데이터를 저장합니다.
   만약 전체 cache의 크기가 MAX_CACHE_SIZE를 초과하면,
   가장 끝에 있는 (사용하지 않고 오래된) 데이터를 삭제하고 저장합니다. */
void add_cache(char *data, int size, char *uri){
	printf("\t###add_cache###\n");
	
	/* if full, remove oldest(last) one */
	while((cache_size + size) > MAX_CACHE_SIZE){ 
		cache *t = head;
		while((t->next->next) != NULL) t = t->next;
		cache_size -= t->next->size;
		free(&(t->next->size));
		free(t->next->data);
		free(t->next->uri);
		free(t->next);
		t->next = NULL;
		printf("oldest cache romoved\n");
	}
	
	/* add cache and add becomes latest(least recently used) */
	cache* add = malloc(sizeof(cache));
	add->data = malloc(size);
	memcpy(add->data, data, size);
	add->uri = malloc((strlen(uri)+1) * sizeof(char));
	strcpy(add->uri, uri);
	add->size = size;
	add->next = head;
	head = add;
	cache_size += size; //sum of datas.
	
	printf("\t\t###cache_size : %d \n",cache_size);

	return;	
}

/* find_in_cache : cache linkedList를 순서대로 조사하며 uri를 비교합니다.
   만약 존재하면 해당 데이터의 포인터를 반환하고 해당데이터를
   cache의 제일 앞(head)로 지정합니다. - Least Recently Used
   그렇지 않으면 NULL을 반환합니다. */
cache* find_in_cache(char *uri){
	printf("\t###find_in_cache###\n");
	cache* ret = head; 
	cache* temp = NULL;
	
	/* when hit, return ret & ret becomes head */
	while(ret!=NULL)
	{
		if(strcmp(ret->uri,uri) == 0) {
			printf("cache hit\n");
			if(head == ret){
				return ret;
			}else {
				temp->next = ret->next;
				ret->next = head;
				head = ret;
				return ret;
			}
		}
		temp = ret;
		ret = ret->next;
	}

	return NULL;

	
}


void error(char *msg){
	perror(msg);
}

/* rdLock, wrLock, unLock
   thread간 cache 데이터 접근에 사용됩니다.
   lock걸린 대상은 rdLock일 경우 다른 thread의 읽기는 허용되지만 쓰기는 안됩니다.
   wrlock일 경우, 둘 다 되지 않습니다. */
void rdLock(pthread_rwlock_t* lock){
	pthread_rwlock_rdlock(lock);
	printf("!!!thread rdLocked!!!\n");
}
void wrLock(pthread_rwlock_t* lock){
	pthread_rwlock_wrlock(lock);
	printf("!!!thread wrLocked!!!\n");
}
void unLock(pthread_rwlock_t* lock){
	pthread_rwlock_unlock(lock);
	printf("!!!thread unLocked!!!\n");
}
