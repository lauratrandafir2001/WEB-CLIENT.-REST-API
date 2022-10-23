
#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "parson.h"
#include <string.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"
#include "utils.h"
#define SERVER "34.241.4.235"
#define REGISTER "/api/v1/tema/auth/register"
#define LOGIN "/api/v1/tema/auth/login"
#define LIBRARY_ACCESS "/api/v1/tema/library/access"
#define GET_BOOKS "/api/v1/tema/library/books"
#define ADD_BOOK "/api/v1/tema/library/books"
#define LOGOUT "/api/v1/tema/auth/logout"
#define PORT 8080

void register_account(char *user, char *password, int socket)
{

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;

    json_object_set_string(root_object, "username", user);

    json_object_set_string(root_object, "password", password);

    serialized_string = json_serialize_to_string_pretty(root_value);

    puts(serialized_string);

    char *message = compute_post_request(SERVER, REGISTER, "application/json", &serialized_string, 1, NULL, 0);

    send_to_server(socket, message);
    char *response = receive_from_server(socket);

    if (response == NULL)
    {
        printf("The server response is NULL.You can not register right now.Please try again later.\n");
    }
    else
    {
        printf("%s", response);
    }
    JSON_Value *json_response = json_parse_string(response);
    char *ok = strstr(response, "Created");

    if (ok == NULL)
    {
        printf("Register was not possible.Please try again later\n");
        free(message);
        free(response);
        json_free_serialized_string(serialized_string);
        json_value_free(root_value);
    }
    else
    {
        printf("Register successful!\n");
        free(message);
        free(response);
        json_free_serialized_string(serialized_string);
        json_value_free(root_value);
    }
}

char *login(char *user, char *password, int socket)
{

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;
    json_object_set_string(root_object, "username", user);
    json_object_set_string(root_object, "password", password);
    serialized_string = json_serialize_to_string_pretty(root_value);
    puts(serialized_string);

    char *message = compute_post_request(SERVER, LOGIN, "application/json", &serialized_string, 1, NULL, 0);
    send_to_server(socket, message);
    char *response = receive_from_server(socket);

    if (response == NULL)
    {
        printf("The server response is NULL.You can not login right now.Please try again later.\n");
    }
    else
    {
        printf("%s", response);
    }
    JSON_Value *json_response = json_parse_string(response);
    char *ok = strstr(response, "OK");

    if (ok == NULL)
    {
        printf("An error occured. Login failed\n");

        free(message);
        free(response);
        json_free_serialized_string(serialized_string);
        json_value_free(root_value);
        return NULL;
    }
    //we take the cookie from response
    char *cookie_start = strstr(response, "connect");
    //we establish the end of the cookie
    char *cookie_big = strtok(cookie_start, ";");
    //we free the memory
    free(message);
    free(response);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    char *cookie = calloc(800, sizeof(char));
    DIE(cookie == NULL, "Memory could not be allocated\n");
    strncpy(cookie, cookie_big, 800 * (sizeof(char)));
    printf("Login successful!\n");
    //we return the cookie
    return cookie;
}
char *enter_library(int socket, int logged, char *cookie)
{
    //we verify if the user is not logged in
    if (logged == 0 && strlen(cookie) == 0)
    {
        printf("You are not logged in.Please log in first!\n");
    }
    //if the user is logged in we return the token JWT
    else if (logged != 0 && strlen(cookie) != 0)
    {

        JSON_Value *root_value = json_value_init_object();
        JSON_Object *root_object = json_value_get_object(root_value);
        char *serialized_string = NULL;
        char *message = compute_get_request(SERVER, LIBRARY_ACCESS, NULL, &cookie, 1);
        send_to_server(socket, message);
        char *response = receive_from_server(socket);
        //verify if the server responded
        if (response == NULL)
        {
            printf("The server response is NULL.You can not enter_library right now.Please try again later.\n");
        }
        else
        {
            printf("%s", response);
        }
        JSON_Value *json_response = json_parse_string(response);
        //we verify if the server response is ok
        char *ok = strstr(response, "OK");
        if (ok == NULL)
        {
            printf("Access to library failed.Please try again later\n");
            free(message);
            free(response);
            return NULL;
        }
        else
        {
            char *token = calloc(800, sizeof(char));
            DIE(token == NULL, "Memory could not be allocated\n");
            //take the token from the response
            char *token_start = strstr(response, "{");
            JSON_Value *val = json_parse_string(token_start);
            JSON_Object *root_object = json_value_get_object(val);
            serialized_string = json_object_get_string(root_object, "token");
            strcpy(token, serialized_string);
            printf("You have entered the library successfully!\n");
            // freeing up memory
            json_value_free(val);
            free(response);
            //returning token
            return token;
        }
    }
}
void view_books(int socket, int logged, char *cookie, char *token)
{
    //we verify if the user is not logged in
    if (logged == 0 || strlen(cookie) == 0)
    {
        printf("You are not logged in.Please log in first!\n");
        return;
    }
    //we verify if the user entered library
    if (token == NULL)
    {
        printf("Please enter library first.\n");
        return;
    }
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;
    char *message = compute_get_request_with_headers(SERVER, GET_BOOKS, NULL, &cookie, 1, token, 1);
    send_to_server(socket, message);
    char *response = receive_from_server(socket);
    //verify if the server responded
    if (response == NULL)
    {
        printf("The server response is NULL.You can not view books right now.Please try again later.\n");
    }
    else
    {
        printf("%s", response);
    }
    char *ok = strstr(response, "OK");
    if (ok == NULL)
    {
        printf("An error occured.You can not view books now\n");

        free(message);
        free(response);
    }
    else
    {
        puts("You successfully viewed books\n");
    }
}
void add_book(int socket, int logged, char *cookie, char *token, char *title, char *author, char *genre, char *publisher, char *page_count)
{
    //we verify if the user is not logged in
    if (logged == 0 || strlen(cookie) == 0)
    {
        printf("You are not logged in.Please log in first!\n");
        return;
    }
    //we verify if the user has entered the library
    if (token == NULL)
    {
        printf("Please enter library first.\n");
        return;
    }
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    //we create the json book object
    json_object_set_string(root_object, "title", title);
    json_object_set_string(root_object, "author", author);
    json_object_set_string(root_object, "genre", genre);
    json_object_set_number(root_object, "page_count", atoi(page_count));
    json_object_set_string(root_object, "publisher", publisher);
    char *serialized_string = json_serialize_to_string_pretty(root_value);
    puts(serialized_string);
    char *message = compute_post_request_with_headers(SERVER, ADD_BOOK, "application/json", &serialized_string, 1, &cookie, 1, token, 1);

    send_to_server(socket, message);

    char *response = receive_from_server(socket);
    //verify if the server responded
    if (response == NULL)
    {
        printf("The server response is NULL.You can not add books right now.Please try again later.\n");
    }
    else
    {
        printf("%s", response);
    }
    char *ok = strstr(response, "OK");

    if (ok == NULL)
    {
        printf("An error occured.Adding book failed\n");

        free(message);
        free(response);
    }
    else
    {
        puts("Book was succesfull added\n");
    }
}

void delete_book(int socket, int logged, char *cookie, char *token, char *id)
{
    //we verify if the user is not logged in
    if (logged == 0 || strlen(cookie) == 0)
    {
        printf("You are not logged in.Please log in first!\n");
        return;
    }
    //we verify if the user has entered the library
    if (token == NULL)
    {
        printf("Please enter library first.\n");
        return;
    }

    //concat the id of the book we want to delete
    char deleted_book[300];
    strcpy(deleted_book, "/api/v1/tema/library/books/");
    strcat(deleted_book, id);
    char *message = compute_delete_request_with_headers(SERVER, deleted_book, NULL, &cookie, 1, token, 1);
    send_to_server(socket, message);
    char *response = receive_from_server(socket);
    //verify if the server responded
    if (response == NULL)
    {
        printf("The server response is NULL.You can not add books right now.Please try again later.\n");
    }
    else
    {
        printf("%s", response);
    }
    char *ok = strstr(response, "OK");

    if (ok == NULL)
    {
        printf("An error occured.Deleting book failed.\n");
        //free memory
        free(message);
        free(response);
    }
    else
    {
        puts("Book was succesfull deleted\n");
    }
}

void view_book(int socket, int logged, char *cookie, char *token, char *id)
{

    //we verify if the user is not logged in
    if (logged == 0 || strlen(cookie) == 0)
    {
        printf("You are not logged in.Please log in first!\n");
        return;
    }
    //we verify if the user has entered the library
    if (token == NULL)
    {
        printf("Please enter library first.\n");
        return;
    }
    //concat the id of the book we want to view
    char wanted_book[300];
    memset(wanted_book, 0, 300);
    strcpy(wanted_book, "/api/v1/tema/library/books/");
    strcat(wanted_book, id);
    char *message = compute_get_request_with_headers(SERVER, wanted_book, NULL, &cookie, 1, token, 1);
    send_to_server(socket, message);
    char *response = receive_from_server(socket);
    //verify if the server responded
    if (response == NULL)
    {
        printf("The server response is NULL.You can not add books right now.Please try again later.\n");
    }
    char *ok = strstr(response, "OK");
    if (ok == NULL)
    {
        printf("An error occured. Viewing books failed\n");
        free(message);
        free(response);
    }
    else
    {

        char *book = basic_extract_json_response(response);
        if (book == NULL)
        {
            // There are no books in the library
            puts("There are no books!\n");
        }
        else
        {
            puts("This is your book: ");
            puts(book);
        }
    }
}

int logout(int sockfd, int logged, char *cookie)
{
    //we verify if the user is not logged in
    if (logged == 0 || strlen(cookie) == 0)
    {
        printf("You are not logged in.Please log in first!\n");
        return 0;
    }
    char *message = compute_get_request(SERVER, LOGOUT, NULL, &cookie, 1);
    send_to_server(sockfd, message);
    char *response = receive_from_server(sockfd);
    char *ok = strstr(response, "OK");
    if (ok == NULL)
    {
        printf("An error occured. Logout failed!\n");
        free(message);
        free(response);

        return 0;
    }
    else
    {
        puts("Logout successfully!");
        free(message);
        free(response);
        return 1;
    }
}

int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd;
    char *current_cookie;
    char *token1 = NULL;
    char *cookie1 = NULL;
    int logged = 0;
    size_t size = 400;
    printf("Please enter a command\n");

    while (1)
    {
        // register
        //citim username si parola
        char *command, command_line[200];
        fgets(command_line, 200, stdin);
        command = strtok(command_line, "\n ");
        //register a user

        if (strcmp(command, "register") == 0)
        {
            sockfd = open_connection(SERVER, PORT, AF_INET, SOCK_STREAM, 0);
            //get username and password from stdin
            char *user = calloc(200, sizeof(char));
            char *password = calloc(200, sizeof(char));
            printf("username=");
            getline(&user, &size, stdin);
            printf("password=");
            getline(&password, &size, stdin);
            register_account(user, password, sockfd);
        }
        //login a user
        else if (strcmp(command, "login") == 0)
        {
            sockfd = open_connection(SERVER, PORT, AF_INET, SOCK_STREAM, 0);
            //get username and password from stdin
            char *user = calloc(200, sizeof(char));
            char *password = calloc(200, sizeof(char));
            printf("username=");
            getline(&user, &size, stdin);
            printf("password=");
            getline(&password, &size, stdin);
            //the function returns a cookie for the current session
            cookie1 = login(user, password, sockfd);
            if (cookie1 != NULL)
            {
                //to make sure this user is logged
                logged = 1;
            }
        }
        //function for the client to access the library
        else if (strcmp(command, "enter_library") == 0)
        {

            sockfd = open_connection(SERVER, PORT, AF_INET, SOCK_STREAM, 0);
            token1 = enter_library(sockfd, logged, cookie1);
        }
        //function for the client to view all the books

        else if (strcmp(command, "get_books") == 0)
        {

            sockfd = open_connection(SERVER, PORT, AF_INET, SOCK_STREAM, 0);
            view_books(sockfd, logged, cookie1, token1);
        }
        //function for the client to add a book

        else if (strcmp(command, "add_book") == 0)
        {
            char *title, *author, *genre, *publisher;
            char *page_count;
            int ok = 1;
            title = (char *)calloc(100, sizeof(char));
            author = (char *)calloc(100, sizeof(char));
            DIE(author == NULL, "Memory could not be allocated\n");
            genre = (char *)calloc(100, sizeof(char));
            DIE(genre == NULL, "Memory could not be allocated\n");
            publisher = (char *)calloc(100, sizeof(char));
            page_count = (char *)calloc(100, sizeof(char));
            DIE(publisher == NULL, "Memory could not be allocated\n");
            size_t size = 100;
            //gets book info from stdin
            printf("title=");
            getline(&title, &size, stdin);

            printf("author=");
            getline(&author, &size, stdin);

            printf("genre=");
            getline(&genre, &size, stdin);

            printf("publisher=");
            getline(&publisher, &size, stdin);

            printf("page_count=");
            getline(&page_count, &size, stdin);
            if (strlen(title) <= 1 || strlen(author) <= 1 || strlen(genre) < 1 || strlen(publisher) < 1 || page_count <= 1)
            {
                printf("The information introduced are wrong. Try again later!\n");
                free(author);
                free(genre);
                free(publisher);
                free(title);
                return;
            }

            sockfd = open_connection(SERVER, PORT, AF_INET, SOCK_STREAM, 0);

            add_book(sockfd, logged, cookie1, token1, title, author, genre, publisher, page_count);
            free(author);
            free(genre);
            free(publisher);
            free(title);
        }
        //function for the client to delete a book
        else if (strcmp(command, "delete_book") == 0)
        {
            char *id;
            //takes id from stdin
            id = (char *)calloc(100, sizeof(char));
            printf("id=");
            size_t size = 100;
            getline(&id, &size, stdin);
            id[strlen(id) - 1] = '\0';
            sockfd = open_connection(SERVER, PORT, AF_INET, SOCK_STREAM, 0);
            delete_book(sockfd, logged, cookie1, token1, id);
        }
        else if (strcmp(command, "get_book") == 0)
        {
            char *id;
            // gets id from stdin
            id = (char *)calloc(100, sizeof(char));
            printf("id=");
            size_t size = 100;
            getline(&id, &size, stdin);
            id[strlen(id) - 1] = '\0';
            sockfd = open_connection(SERVER, PORT, AF_INET, SOCK_STREAM, 0);
            view_book(sockfd, logged, cookie1, token1, id);
        }
        else if (strcmp(command, "logout") == 0)
        {

            sockfd = open_connection(SERVER, PORT, AF_INET, SOCK_STREAM, 0);
            int ok = logout(sockfd, logged, cookie1);
            if (ok == 0)
            {
                //if an error occured
                printf("Logout impossible");
                close(sockfd);
                continue;
            }
            else
            {
                //we free the memory
                if (cookie1 != NULL && strlen(cookie1) != 0)
                {

                    free(cookie1);
                }
                if (token1 != NULL && strlen(token1) != 0)
                {
                    free(token1);
                }
                logged = 0;
                cookie1 = NULL;
                token1 = NULL;

                close(sockfd);
            }
        }
        else if (strcmp(command, "exit") == 0)
        {
            //we free the memory
            if (cookie1 != NULL && strlen(cookie1) != 0)
            {

                free(cookie1);
            }
            if (token1 != NULL && strlen(token1) != 0)
            {
                free(token1);
            }
            logged = 0;
            cookie1 = NULL;
            token1 = NULL;
            break;
        }

        else
        {
            puts("You entered a wrong command.Please try again\n");
        }
    }

    close(sockfd);

    return 0;
}