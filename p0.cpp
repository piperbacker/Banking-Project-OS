#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include <fstream>

#include "defs.h"

struct stats
{
    int no_deposits;    // # of deposits into the account (not include if rejected)
    int no_withdrawals; // # of withdrawals from the account (not include if rejected)
                        // a transfer is counted as one withdrawal and one deposit to the corresponding accounts
    int no_rejected;    // # of rejected transactions
    int balance;        // account balance
};

// shared global variable
stats checking_account;
stats savings_account;

//individual tracking variables
stats th_checking[10];
stats th_savings[10];

void *banking(void *p)
{
    parameters *params = (parameters *)p;
    int loop = params->loop_count;
    int i = params->thr_index;

    std::ofstream file;
    file.open(params->filename);

    for (int j = 0; j < loop; j++)
    {
        int transaction = rand() % 6 + 1;

        // DEPOSIT CHECKING
        if (transaction == 1)
        {
            int amount = rand() % 100 + 50;
            int x = checking_account.balance;
            x += amount;
            usleep(rand() % 200000);
            checking_account.balance = x;
            th_checking[i].balance += amount;
           
            int y = checking_account.no_deposits;
            y++;
            usleep(rand() % 200000);
            checking_account.no_deposits = y;
            th_checking[i].no_deposits++;

            file << "DEPOSIT CHECKING " << amount << std::endl;
            printf("DEPOSIT CHECKING %d \n", amount);
        }
        // WITHDRAWAL CHECKING
        else if (transaction == 2)
        {
            int amount = rand() % 100 + 50;
            if (th_checking[i].balance > amount)
            {
                int x = checking_account.balance;
                x -= amount;
                usleep(rand() % 200000);
                checking_account.balance = x;
                th_checking[i].balance -= amount;

                int y = checking_account.no_withdrawals;
                y++;
                usleep(rand() % 200000);
                checking_account.no_withdrawals = y;
                th_checking[i].no_withdrawals++;

                file << "WITHDRAWAL CHECKING " << amount << std::endl;
                printf("WITHDRAWAL CHECKING %d \n", amount);
            }
            else
            {
                int y = checking_account.no_rejected;
                y++;
                usleep(rand() % 200000);
                checking_account.no_rejected = y;
                th_checking[i].no_rejected++;

                file << "WITHDRAWAL CHECKING " << amount << " (REJECTED)" << std::endl;
                printf("WITHDRAWAL CHECKING %d (REJECTED) \n", amount);
            }
        }
        // DEPOSIT SAVINGS
        if (transaction == 3)
        {
            int amount = rand() % 150 + 100;
            int x = savings_account.balance;
            x += amount;
            usleep(rand() % 200000);
            savings_account.balance = x;
            th_savings[i].balance += amount;

            int y = savings_account.no_deposits;
            y++;
            usleep(rand() % 200000);
            savings_account.no_deposits = y;
            th_savings[i].no_deposits++;

            file << "DEPOSIT SAVINGS " << amount << std::endl;
            printf("DEPOSIT SAVINGS %d \n", amount);
        }
        // WITHDRAWAL SAVINGS
        else if (transaction == 4)
        {
            int amount = rand() % 150 + 100;
            if (th_savings[i].balance > amount)
            {
                int x = savings_account.balance;
                x -= amount;
                usleep(rand() % 200000);
                savings_account.balance = x;
                th_savings[i].balance -= amount;

                int y = savings_account.no_withdrawals;
                y++;
                usleep(rand() % 200000);
                savings_account.no_withdrawals = y;
                th_savings[i].no_withdrawals++;

                file << "WITHDRAWAL SAVINGS " << amount << std::endl;
                printf("WITHDRAWAL SAVINGS %d \n", amount);
            }
            else
            {
                int x = savings_account.no_rejected;
                x++;
                usleep(rand() % 200000);
                savings_account.no_rejected = x;
                th_savings[i].no_rejected++;

                file << "WITHDRAWAL SAVINGS " << amount << " (REJECTED)" << std::endl;
                printf("WITHDRAWAL SAVINGS %d (REJECTED) \n", amount);
            }
        }
        // TRANSFER CHECKING TO SAVINGS
        else if (transaction == 5)
        {
            int amount = rand() % 200 + 100;
            if (th_checking[i].balance > amount)
            {
                int x = checking_account.balance;
                x -= amount;
                usleep(rand() % 200000);
                checking_account.balance = x;
                th_checking[i].balance -= amount;

                int y = checking_account.no_withdrawals;
                y++;
                usleep(rand() % 200000);
                checking_account.no_withdrawals = y;
                th_checking[i].no_withdrawals++;

                int w = savings_account.balance;
                w += amount;
                usleep(rand() % 200000);
                savings_account.balance = w;
                th_savings[i].balance += amount;

                int z = savings_account.no_deposits;
                z++;
                usleep(rand() % 200000);
                savings_account.no_deposits = z;
                th_savings[i].no_deposits++;

                file << "TRANSFER CHECKING TO SAVINGS " << amount << std::endl;
                printf("TRANSFER CHECKING TO SAVINGS  %d \n", amount);
            }
            else
            {
                int x = checking_account.no_rejected;
                x++;
                usleep(rand() % 200000);
                checking_account.no_rejected = x;
                th_checking[i].no_rejected++;

                file << "TRANSFER CHECKING TO SAVINGS " << amount << " (REJECTED)" << std::endl;
                printf("TRANSFER CHECKING TO SAVINGS %d (REJECTED) \n", amount);
            }
        }
        // TRANSFER SAVINGS TO CHECKING
        else if (transaction == 6)
        {
            int amount = rand() % 200 + 100;

            if (th_savings[i].balance > amount)
            {
                int x = savings_account.balance;
                x -= amount;
                usleep(rand() % 200000);
                savings_account.balance = x;
                th_savings[i].balance -= amount;

                int y = savings_account.no_withdrawals;
                y++;
                usleep(rand() % 200000);
                savings_account.no_withdrawals = y;
                th_savings[i].no_withdrawals++;

                int w = checking_account.balance;
                w += amount;
                usleep(rand() % 200000);
                checking_account.balance = w;
                th_checking[i].balance += amount;

                int z = checking_account.no_deposits;
                z++;
                usleep(rand() % 200000);
                checking_account.no_deposits = z;
                th_checking[i].no_deposits++;

                file << "TRANSFER SAVINGS TO CHECKING " << amount << std::endl;
                printf("TRANSFER SAVINGS TO CHECKING %d \n", amount);
            }
            else
            {
                int x = savings_account.no_rejected;
                x++;
                usleep(rand() % 200000);
                savings_account.no_rejected = x;
                th_savings[i].no_rejected++;

                file << "TRANSFER SAVINGS TO CHECKING " << amount << " (REJECTED)" << std::endl;
                printf("TRANSFER SAVINGS TO CHECKING %d (REJECTED) \n", amount);
            }
        }
        
        usleep(rand() % 200000); // to increase probability of race condition
    }

    // print final results to output file and console
    file << "\nThread " << i+1 << " stats: " << std::endl;
    file << "CHECKING BALANCE " << th_checking[i].balance << std::endl;
    file << "CHECKING DEPOSITS " << th_checking[i].no_deposits << std::endl;
    file << "CHECKING WITHDRAWALS " << th_checking[i].no_withdrawals << std::endl;
    file << "CHECKING REJECTED TRANSACTIONS " << th_checking[i].no_rejected << std::endl;

    file << "SAVINGS BALANCE " << th_savings[i].balance << std::endl;
    file << "SAVINGS DEPOSITS " << th_savings[i].no_deposits << std::endl;
    file << "SAVINGS WITHDRAWALS " << th_savings[i].no_withdrawals << std::endl;
    file << "SAVINGS REJECTED TRANSACTIONS " << th_savings[i].no_rejected << "\n"
         << std::endl;

    printf("\nThread %d stats: \n", i+1);
    printf("CHECKING BALANCE %d \n", th_checking[i].balance);
    printf("CHECKING DEPOSITS %d \n", th_checking[i].no_deposits);
    printf("CHECKING WITHDRAWALS %d \n", th_checking[i].no_withdrawals);
    printf("CHECKING REJECTED TRANSACTIONS %d \n", th_checking[i].no_rejected);

    printf("SAVINGS BALANCE %d \n", th_savings[i].balance);
    printf("SAVINGS DEPOSITS %d \n", th_savings[i].no_deposits);
    printf("SAVINGS WITHDRAWALS %d \n", th_savings[i].no_withdrawals);
    printf("SAVINGS REJECTED TRANSACTIONS %d \n\n", th_savings[i].no_rejected);

    file.close();
    pthread_exit(0);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: %s loop_count\n", argv[0]);
        exit(1);
    }

    int loop_count = atoi(argv[1]);

    if (loop_count == 0)
    {
        printf("Usage: %s max_loop_count\n", argv[0]);
        exit(1);
    }

    // creating 10 threads
    pthread_t tr1;
    pthread_t tr2;
    pthread_t tr3;
    pthread_t tr4;
    pthread_t tr5;
    pthread_t tr6;
    pthread_t tr7;
    pthread_t tr8;
    pthread_t tr9;
    pthread_t tr10;

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    // pass loop_count, filename, and thread index to thread
    parameters *p1_params = new parameters;
    p1_params->loop_count = loop_count;
    p1_params->filename = (char *)"t1.txt";
    p1_params->thr_index = 0;

    parameters *p2_params = new parameters;
    p2_params->loop_count = loop_count;
    p2_params->filename = (char *)"t2.txt";
    p2_params->thr_index = 1;

    parameters *p3_params = new parameters;
    p3_params->loop_count = loop_count;
    p3_params->filename = (char *)"t3.txt";
    p3_params->thr_index = 2;

    parameters *p4_params = new parameters;
    p4_params->loop_count = loop_count;
    p4_params->filename = (char *)"t4.txt";
    p4_params->thr_index = 3;

    parameters *p5_params = new parameters;
    p5_params->loop_count = loop_count;
    p5_params->filename = (char *)"t5.txt";
    p5_params->thr_index = 4;

    parameters *p6_params = new parameters;
    p6_params->loop_count = loop_count;
    p6_params->filename = (char *)"t6.txt";
    p6_params->thr_index = 5;

    parameters *p7_params = new parameters;
    p7_params->loop_count = loop_count;
    p7_params->filename = (char *)"t7.txt";
    p7_params->thr_index = 6;

    parameters *p8_params = new parameters;
    p8_params->loop_count = loop_count;
    p8_params->filename = (char *)"t8.txt";
    p8_params->thr_index = 7;

    parameters *p9_params = new parameters;
    p9_params->loop_count = loop_count;
    p9_params->filename = (char *)"t9.txt";
    p9_params->thr_index = 8;

    parameters *p10_params = new parameters;
    p10_params->loop_count = loop_count;
    p10_params->filename = (char *)"t10.txt";
    p10_params->thr_index = 9;

    pthread_create(&tr1, &attr, banking, (void *)p1_params);
    pthread_create(&tr2, &attr, banking, (void *)p2_params);
    pthread_create(&tr3, &attr, banking, (void *)p3_params);
    pthread_create(&tr4, &attr, banking, (void *)p4_params);
    pthread_create(&tr5, &attr, banking, (void *)p5_params);
    pthread_create(&tr6, &attr, banking, (void *)p6_params);
    pthread_create(&tr7, &attr, banking, (void *)p7_params);
    pthread_create(&tr8, &attr, banking, (void *)p8_params);
    pthread_create(&tr9, &attr, banking, (void *)p9_params);
    pthread_create(&tr10, &attr, banking, (void *)p10_params);

    pthread_join(tr1, NULL);
    pthread_join(tr2, NULL);
    pthread_join(tr3, NULL);
    pthread_join(tr4, NULL);
    pthread_join(tr5, NULL);
    pthread_join(tr6, NULL);
    pthread_join(tr7, NULL);
    pthread_join(tr8, NULL);
    pthread_join(tr9, NULL);
    pthread_join(tr10, NULL);

    printf("\nShared Stats: \n");
    printf("CHECKING BALANCE %d \n", checking_account.balance);
    printf("CHECKING DEPOSITS %d \n", checking_account.no_deposits);
    printf("CHECKING WITHDRAWALS %d \n", checking_account.no_withdrawals);
    printf("CHECKING REJECTED TRANSACTIONS %d \n\n", checking_account.no_rejected);

    printf("SAVINGS BALANCE %d \n", savings_account.balance);
    printf("SAVINGS DEPOSITS %d \n", savings_account.no_deposits);
    printf("SAVINGS WITHDRAWALS %d \n", savings_account.no_withdrawals);
    printf("SAVINGS REJECTED TRANSACTIONS %d \n\n", savings_account.no_rejected);

    // checking for race conditions
    int c_sum = 0;
    int c_deposits = 0;
    int c_withdrawals = 0;
    int c_rejected = 0;

    int s_sum = 0;
    int s_deposits = 0;
    int s_withdrawals = 0;
    int s_rejected = 0;

    for (int i = 0; i < 10; i++)
    {
        c_sum += th_checking[i].balance;
        c_deposits += th_checking[i].no_deposits;
        c_withdrawals += th_checking[i].no_withdrawals;
        c_rejected += th_checking[i].no_rejected;

        s_sum += th_savings[i].balance;
        s_deposits += th_savings[i].no_deposits;
        s_withdrawals += th_savings[i].no_withdrawals;
        s_rejected += th_savings[i].no_rejected;
    }


    printf("\nIndividual Sum Stats: \n");
    printf("CHECKING BALANCE %d \n", c_sum);
    printf("CHECKING DEPOSITS %d \n", c_deposits);
    printf("CHECKING WITHDRAWALS %d \n", c_withdrawals);
    printf("CHECKING REJECTED TRANSACTIONS %d \n\n", c_rejected);

    printf("SAVINGS BALANCE %d \n", s_sum);
    printf("SAVINGS DEPOSITS %d \n", s_deposits);
    printf("SAVINGS WITHDRAWALS %d \n", s_withdrawals);
    printf("SAVINGS REJECTED TRANSACTIONS %d \n\n", s_rejected);

    // check if race conditions occured in checking balance
    if (checking_account.balance != c_sum)
    {
        printf("Race conditions occured in checking account balance.\n");
        printf("Shared Checking Balance: %d\n", checking_account.balance);
        printf("Individual Sum Checking Balance: %d\n", c_sum);
        printf("Balance Difference: %d\n\n", (c_sum - checking_account.balance));
    }
    // check if race conditions occured in checking no_deposits
    if (checking_account.no_deposits != c_deposits)
    {
        printf("Race conditions occured in checking account no_deposits.\n");
        printf("Shared Checking Deposits: %d\n", checking_account.no_deposits);
        printf("Individual Sum Checking Deposits: %d\n", c_deposits);
        printf("Deposits Difference: %d\n\n", (c_deposits - checking_account.no_deposits));
    }
    // check if race conditions occured in checking no_withdrawals
    if (checking_account.no_withdrawals != c_withdrawals)
    {
        printf("Race conditions occured in checking account no_withdrawals.\n");
        printf("Shared Checking Withdrawals: %d\n", checking_account.no_withdrawals);
        printf("Individual Sum Checking Withdrawals: %d\n", c_withdrawals);
        printf("Withdrawals Difference: %d\n\n", (c_withdrawals - checking_account.no_withdrawals));
    }
    // check if race conditions occured in checking no_rejected
    if (checking_account.no_rejected != c_rejected)
    {
        printf("Race conditions occured in checking account no_rejected.\n");
        printf("Shared Checking Rejected: %d\n", checking_account.no_rejected);
        printf("Individual Sum Checking Rejected: %d\n", c_rejected);
        printf("Rejected Difference: %d\n\n", (c_rejected - checking_account.no_rejected));
    }


    // check if race conditions occured in savings balance
    if (savings_account.balance != s_sum)
    {
        printf("Race conditions occured in savings account balance.\n");
        printf("Shared Savings Balance: %d\n", savings_account.balance);
        printf("Individual Sum Savings Balance: %d\n", s_sum);
        printf("Balance Difference: %d\n\n", (s_sum - savings_account.balance));
    }
    // check if race conditions occured in savings no_deposits
    if (savings_account.no_deposits != s_deposits)
    {
        printf("Race conditions occured in savings account no_deposits.\n");
        printf("Shared Savings Deposits: %d\n", savings_account.no_deposits);
        printf("Individual Sum Savings Deposits: %d\n", s_deposits);
        printf("Deposits Difference: %d\n\n", (s_deposits - savings_account.no_deposits));
    }
    // check if race conditions occured in savings no_withdrawals
    if (savings_account.no_withdrawals != s_withdrawals)
    {
        printf("Race conditions occured in savings account no_withdrawals.\n");
        printf("Shared Savings Withdrawals: %d\n", savings_account.no_withdrawals);
        printf("Individual Sum Savings Withdrawals: %d\n", s_withdrawals);
        printf("Withdrawals Difference: %d\n\n", (s_withdrawals - savings_account.no_withdrawals));
    }
    // check if race conditions occured in savings no_rejected
    if (savings_account.no_rejected != s_rejected)
    {
        printf("Race conditions occured in savings account no_rejected.\n");
        printf("Shared Savings Rejected: %d\n", savings_account.no_rejected);
        printf("Individual Sum Savings Rejected: %d\n", s_rejected);
        printf("Rejected Difference: %d\n\n", (s_rejected - savings_account.no_rejected));
    }

    if((checking_account.balance == c_sum) && savings_account.balance == s_sum) {
        printf("No race conditions have occured.");
    }
}
