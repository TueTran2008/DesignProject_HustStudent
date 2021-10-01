/* Simple HTTP Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_eth.h"
//#include "protocol_examples_common.h"

#include <esp_http_server.h>

/* A simple example that demonstrates how to create GET and POST
 * handlers for the web server.
 */

static const char *TAG = "example";
static httpd_handle_t server = NULL;


char sensor_data_str[100] = "{\"temperature\": \"100\",\"humidity\": \"100\",\"distance\": \"100\"}";
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");


/* An HTTP GET handler */
static esp_err_t hello_get_handler(httpd_req_t *req)
{
    //char*  buf;
    //const char* resp_str = (const char*)"Hello World";
    httpd_resp_set_type(req,"text/html");
    httpd_resp_send(req, (const char *)index_html_start, index_html_end - index_html_start);
    //if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
      //  ESP_LOGI(TAG, "Request headers lost");
    //}
    return ESP_OK;
}

static const httpd_uri_t hello = {
    .uri       = "/dht11",
    .method    = HTTP_GET,
    .handler   = hello_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = NULL
};




static esp_err_t get_data(httpd_req_t *req)
{
    
    const char* dht_str = (const char*)sensor_data_str;
    httpd_resp_send(req,dht_str, strlen(dht_str));
    return ESP_OK;
}

static const httpd_uri_t get_data_dht11 = {
    .uri       = "/getdht11data",
    .method    = HTTP_GET,
    .handler   = get_data,

    .user_ctx  = NULL
};


esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    if (strcmp("/dht11", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/hello URI is not available");
        /* Return ESP_OK to keep underlying socket open */
        return ESP_OK;
    }
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}
void start_webserver(void)
{

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &hello);
        httpd_register_uri_handler(server, &get_data_dht11);
        httpd_register_err_handler(server, HTTPD_404_NOT_FOUND,http_404_error_handler);
    }
    else
    {
        ESP_LOGI(TAG, "Error starting server!");
    }
}

void stop_webserver(void)
{
    httpd_stop(server);
}
