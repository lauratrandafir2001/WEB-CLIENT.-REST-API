#WEB CLIENT. REST API

#About

Communication Protocols Course

May 2022

Student: Trandafir Laura;

Functionality: using the HTTP protocol, we simulate a server-client interaction where the client can register,
                login, logout, enter a virtual library, and add, delete or view info about its books;


#Implementation:
The user enters a command from stdin.
The commands could be:

register: 

-the user registers a new account with the username
and password is given on the keyboard
-if the username is already taken or the credentials
are not valid will receive an error
-we create the JSON object and send a request to the server
-if the server responds without any errors and prints a message 
on the screen

login:
      
-a user can log in if it's already registered and the credentials are correct
-we will send a request to the server and based on the response
we will print a message on the screen and the actual response from the server
-if the response from the server is ok we will return a session cookie
-if the user is logged(the cookie returns something that is not null
then the variable logged will be 1)

enter_library:

-we verify if the user is logged
-if the user is logged we send a request to the server using the cookie
-if the answer from the server is ok then we extract the token
-we will print a message on the screen if it's an error or not

view_books:
-if the user requests to get_books(to view all the books from the library)
-we will first verify if it's logged and if he/she entered the library
-if the user already did these things then we will use 
compute_get_request_with_headers to send a request to the server
where we add the cookies and token given previously;
-if the response from the server is not NULL we print it on screen
-based on the content of the response we print an error message
or success message and the books from the library;

add_book: 

-when the user wants to add a book
-we will take the info on the book he/she wants to add
from stdin using getline function 
-we will verify if the info is correct(the title/author/genre etc
contains more than 1 character and if the page number(page_count is 
bigger than 0 and contains numbers)
-we will call the function add_book
-we will first verify if it's logged and if he/she entered the library
-if the user already did these things then we will 
create the JSON object using the info given previously and call 
compute_post_request_with_headers to send a request to the server
where we add the cookies and token given previously;
-if the response from the server is not NULL we print it on screen
-based on the content of the response we print an error message
or success message;

delete_book:

-when the user wants to delete a book
-we take the id of the book from stdin using getline
-we call the function delete_book
-we will first verify if it's logged and if he/she entered the library
-if the user already did these things then we will concatenate the id to the 
string URL and send the request to the server
-if the response from the server is not NULL we print it on screen
-based on the content of the response we print an error message
or success message;

view_book:
-when the user wants to see a certain book
-we will take the book id from stdin
-we call the function view_book
-we will first verify if it's logged and if he/she entered the library
-if the user already did these things then we will concatenate the id to the 
string URL and send the request to the server using compute_get_request_with_headers
-if the response from the server is not NULL we print it on screen
-based on the content of the response we print an error message
or success message and the info about the book she/he wanted;

logout:
-when the user wants to logout
-we first verify if he/she is logged in
-we send a request to the server using compute_get_request
-if the response from the server is not NULL we print it on screen
-based on the content of the response we print an error message
or success message and return 0 if it's an error and 1 if it's not;
-if the function returns 1 then we will free token and cookie 
-else we will close the socket
                
exit:
-when the user wants to close the program
-we will free the memory and break the while loop from the main

other: 
-if the user introduces other commands the program will
show an error message      



                        

#Programs to install for the app to work:

-parson library for the JSON parsing
-utils for DIE function to see if the memory was allocated correctly
