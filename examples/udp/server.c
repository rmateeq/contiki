#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

#include "net/ip/uip.h"
#include "net/rpl/rpl.h"

#include "net/netstack.h"
#include "dev/button-sensor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"
#include <string.h>

//#define DEBUG 1
//#if DEBUG
#include <stdio.h>
//#define PRINTF(...) printf(__VA_ARGS__)
//#define PRINT6ADDR(addr) PRINTF(" %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x ", ((u8_t *)addr)[0], ((u8_t *)addr)[1], ((u8_t *)addr)[2], ((u8_t *)addr)[3], ((u8_t *)addr)[4], ((u8_t *)addr)[5], ((u8_t *)addr)[6], ((u8_t *)addr)[7], ((u8_t *)addr)[8], ((u8_t *)addr)[9], ((u8_t *)addr)[10], ((u8_t *)addr)[11], ((u8_t *)addr)[12], ((u8_t *)addr)[13], ((u8_t *)addr)[14], ((u8_t *)addr)[15])
//#define PRINTLLADDR(lladdr) PRINTF(" %02x:%02x:%02x:%02x:%02x:%02x ",(lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3],(lladdr)->addr[4], (lladdr)->addr[5])
//#else
//#define PRINTF(...)
//#define PRINT6ADDR(addr)
//#define PRINTLLADDR(addr)
//#endif

#define UDP_IP_BUF   ((struct uip_udpip_hdr *)&uip_buf[UIP_LLH_LEN])

#define MAX_PAYLOAD_LEN 120

static struct uip_udp_conn *server_conn;

PROCESS(udp_server_process, "UDP server process");
AUTOSTART_PROCESSES(&udp_server_process);
/*---------------------------------------------------------------------------*/
static void
tcpip_handler(void)
{
  static int seq_id;
  char buf[MAX_PAYLOAD_LEN];

  if(uip_newdata()) {
    ((char *)uip_appdata)[uip_datalen()] = 0;
    printf("Server received: '%s' from ", (char *)uip_appdata);
    PRINT6ADDR(&UDP_IP_BUF->srcipaddr);
    PRINTF("\n");

    uip_ipaddr_copy(&server_conn->ripaddr, &UDP_IP_BUF->srcipaddr);
    server_conn->rport = UDP_IP_BUF->srcport;
    PRINTF("Responding with message: ");
    sprintf(buf, "Hello from the server! (%d)", ++seq_id);
    PRINTF("%s\n", buf);

    uip_udp_packet_send(server_conn, buf, strlen(buf));
    /* Restore server connection to allow data from any node */
    memset(&server_conn->ripaddr, 0, sizeof(server_conn->ripaddr));
    server_conn->rport = 0;
  }
}
/*---------------------------------------------------------------------------*/
static void
print_local_addresses(void)
{
  int i;
  uint8_t state;

  PRINTF("Server IPv6 addresses: \n");
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    if(state == ADDR_TENTATIVE || state == ADDR_PREFERRED) {
      PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
      PRINTF("\n");
    }
  }
}
static void
print_addr2(const uip_ipaddr_t *ip_addr) {
    int i;
    for (i = 0; i <= 15; i++) {
        printf("%02x ", ip_addr->u8[i]);
    }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data)
{
  uip_ipaddr_t saddr;
  static struct etimer timer;
  //PRINTF("UDP server started\n");
  //printf("UDP server started\n");
  PROCESS_BEGIN();
  //while(1){
  //etimer_set(&timer, CLOCK_SECOND * 5);
  uip_ip6addr(&saddr, 0xcccc,0,0,0,0x0212,0x4b00,0x09df,0x1bc6);
    //uip_gethostaddr(&saddr);
  print_addr2(&saddr);
  //PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    //etimer_reset(&timer);
  //}
    // ,0x2001,0x06a8,0x1d80, 0x2011, 0xbaae, 0xedff, 0xfe75, 0xb97b
  //uip_gethostaddr(saddr);
  //print_addr2(saddr);
  //PRINTF("UDP server started\n");
  printf("UDP server started\n");

  //etimer_set(&timer, CLOCK_SECOND * 5);

  // wait 3 second, in order to have the IP addresses well configured
  
  printf("1st");
  // wait until the timer has expired
  //PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
  
  print_local_addresses();

 
  // set NULL and 0 as IP address and port to accept packet from any node and any srcport.
  server_conn = udp_new(&saddr, UIP_HTONS(0), NULL);
//      print_addr2(&saddr);
  udp_bind(server_conn, UIP_HTONS(3000));
//  while(1){
 //   etimer_set(&timer, CLOCK_CONF_SECOND*5);

 // etimer_reset(&timer);
  //}

  PRINTF("Server listening on UDP port %u\n", UIP_HTONS(server_conn->lport));
  while(1) {
    printf("inside while..\n");
    PROCESS_YIELD();
    printf("yielded..\n");
    if(ev == tcpip_event) {
      printf("inside if..");
      tcpip_handler();
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
