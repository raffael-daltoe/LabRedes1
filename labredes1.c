/*-------------------------------------------------------------*/
/* Exemplo Socket Raw - Captura pacotes recebidos na interface */
/*-------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>

/* Diretorios: net, netinet, linux contem os includes que descrevem */
/* as estruturas de dados do header dos protocolos   	  	        */

#include <net/if.h>  //estrutura ifr
#include <netinet/ether.h> //header ethernet
#include <netinet/in.h> //definicao de protocolos
#include <arpa/inet.h> //funcoes para manipulacao de enderecos IP

#include <netinet/in_systm.h> //tipos de dados

#include <netinet/ip.h>

#define BUFFSIZE 1518



// Atencao!! Confira no /usr/include do seu sisop o nome correto
// das estruturas de dados dos protocolos.

  unsigned char buff1[BUFFSIZE]; // buffer de recepcao

  int sockd;
  int on;
  struct ifreq ifr;

int media(){

}

void zerabuff(){
	int i=0;
	while(i<1518){
		buff1[i]='Z';
		i++;
	}
}

int verificatam(){
	int i=0;
	int tam=0;
	while(buff1[i]!='Z'){
		tam++;
		i++;
	}
	printf("tam do pacote = [%d]\n",tam);
	return tam;
}

void imprime_MAC(){
	printf("MAC Destino: %x:%x:%x:%x:%x:%x \n", buff1[0],buff1[1],buff1[2],buff1[3],buff1[4],buff1[5]);
	printf("MAC Origem:  %x:%x:%x:%x:%x:%x \n\n", buff1[6],buff1[7],buff1[8],buff1[9],buff1[10],buff1[11]);
}

void verificaprotocolo(){
    struct ether_header* eth_cabecalho = (struct ether_header*) buff1;
	if (ntohs(eth_cabecalho->ether_type) == ETHERTYPE_ARP){
		printf("Este é um protocolo ARP!\n");
		imprime_MAC();
	}
	if (ntohs(eth_cabecalho->ether_type) == ETHERTYPE_IP){
		struct iphdr *ip_cabecalho = (struct iphdr*) (buff1 + sizeof(struct ether_header));
		if(ip_cabecalho->protocol == IPPROTO_TCP){
			printf("Este é um protocolo TCP\n");
			imprime_MAC();
		}
	}

}

int main(int argc,char *argv[])
{
    /* Criacao do socket. Todos os pacotes devem ser construidos a partir do protocolo Ethernet. */
    /* De um "man" para ver os parametros.*/
    /* htons: converte um short (2-byte) integer para standard network byte order. */
    if((sockd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
       printf("Erro na criacao do socket.\n");
       exit(1);
    }

	// O procedimento abaixo eh utilizado para "setar" a interface em modo promiscuo
	strcpy(ifr.ifr_name, "wlp2s0");
	if(ioctl(sockd, SIOCGIFINDEX, &ifr) < 0)
		printf("erro no ioctl!");
	ioctl(sockd, SIOCGIFFLAGS, &ifr);
	ifr.ifr_flags |= IFF_PROMISC;
	ioctl(sockd, SIOCSIFFLAGS, &ifr);

	int i=0,tam=0,qtd_pacotes=0;
	int tam_min=5000,tam_max=0,LIM=0;
	float tam_med = 0;
	//printf("Quantos pacotes você deseja capturar?\n");
	//scanf("%d",&LIM);
	LIM = 100;
	// recepcao de pacotes
	while (1) {
		zerabuff();
		recv(sockd,(char *) &buff1, sizeof(buff1), 0x0);
		tam = verificatam();

		qtd_pacotes++;
		tam_med=tam_med+tam;

		if(tam_min > tam)tam_min = tam;
		if(tam_max < tam)tam_max = tam;
		
		tam=0;
		verificaprotocolo();
		// impress�o do conteudo - exemplo Endereco Destino e Endereco Origem

		if(qtd_pacotes == LIM)break;
	}
	tam_med = tam_med / qtd_pacotes;
	printf("Tamanho medio dos pacotes: [%f] \n Pacote de Maior tamanho: [%d] \n Pacote de Menor tamanho: [%d]\n",tam_med,tam_max,tam_min);
}
