Popescu Bogdan
324CC
Tema 2 PC

    Implementare:

    Implementarea temei mi-a luat aproximativ 3 zile:
        -prima zi am creat legarea stdin-tcp, cu mesajul citit de la tastatura;
        -a doua zi am creat legarea tcp-server si udp-server
        -a treia zi am creat legarea server-tcp
    
    Nu am implementat partea de Store&Forward.

    Server:
        
        -pentru pastrarea informatiilor despre mesaje folosesc un vector
        de structuri topic, fiecare topic avand un vector cu clientii
        lui(in care retin fd); mai folosesc, dar doar pentru afisare, 
        un vector in care retin ID-ul clientilor pentru a avea grija
        de posibila duplicare si pentru a-l afisa la deconectare

        -se creeaza socketi pentru conexiunile UPD si TCP

        -serverul ruleaza pana cand se primeste de la tastatura comanda exit


        -serverul primeste mesajele de la UDP si le converteste 
        in formatul pe care il va transmite la TCP, dupa care il trimite la toti
        subscriberii abonati la topicul respectiv

        -in cazul unei cereri de conexiune TCP se verifica duplicitatea si programul
        se inchide daca avem 2 ID-uri identice

        -in cazul comenzii de subscribe/unsubscribe, se adauga/sterge fd-ul asociat 
        subscriberului in topicul respectiv.

    Subscriber:
        -se creeaza un socket de comunicare cu serverul
        
        -ruleaza pana cand se primeste de la tastatura comanda exit;

        -trimite comanda subscribe/unsubscribe serverului

        -primeste de la server ce trebuie sa afiseze la stdout

    
    