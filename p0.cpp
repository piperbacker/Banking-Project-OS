#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include <fstream>

#include "defs.h"

// figure out count, how it works etc
// see if you should move the creation of threads before if statements checking loop count = 0
// put back exit(0)?

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

int buffer[BUF_SIZE];
int count = 0;
//int item_id = 900; // unique item id
//int put_index = 0; // deposit an item at this index
//int get_index = 0; // withdraw an item from this index

sem_t buffer_access;
extern void *banking(void *);

void *banking(void *p)
{
    //printf("checking started with balance = %d\n", checking_account.balance);
    parameters *params = (parameters *)p;
    int loop = params->loop_count; 
    int i = params->thr_index;

    std::ofstream file;
    file.open(params->filename);

    for (int j = 0; j < loop; j++)
    {

        //while (count >= BUF_SIZE) ; // wait

        int transaction = rand() % 6 + 1;

        // DEPOSIT CHECKING
        if (transaction == 1)
        {
            int amount = rand() % 100 + 50;
            checking_account.balance += amount;
            th_checking[i].balance += amount;

            checking_account.no_deposits++;
            th_checking[i].no_deposits++;

            file << "DEPOSIT CHECKING " << amount << std::endl;
            printf("DEPOSIT CHECKING %d \n", amount);
        }
        // WITHDRAWAL CHECKING
        else if (transaction == 2)
        {
            int amount = rand() % 100 + 50;
            if (th_checking[i].balance >= amount)
            {
                checking_account.balance -= amount;
                th_checking[i].balance -= amount;

                checking_account.no_withdrawals++;
                th_checking[i].no_withdrawals++;

                file << "WITHDRAWAL CHECKING " << amount << std::endl;
                printf("WITHDRAWAL CHECKING %d \n", amount);
            }
            else
            {
                checking_account.no_rejected++;
                th_checking[i].no_rejected++;

                file << "WITHDRAWAL CHECKING " << amount << " (REJECTED)" << std::endl;
                printf("WITHDRAWAL CHECKING %d (REJECTED) \n", amount);
            }
        }
        // DEPOSIT SAVINGS
        if (transaction == 3)
        {
            int amount = rand() % 150 + 100;
            savings_account.balance += amount;
            th_savings[i].balance += amount;

            savings_account.no_deposits++;
            th_savings[i].no_deposits++;

            file << "DEPOSIT SAVINGS " << amount << std::endl;
            printf("DEPOSIT SAVINGS %d \n", amount);
        }
        // WITHDRAWAL SAVINGS
        else if (transaction == 4)
        {
            int amount = rand() % 150 + 100;
            if (th_savings[i].balance >= amount)
            {
                savings_account.balance -= amount;
                th_savings[i].balance -= amount;

                savings_account.no_withdrawals++;
                th_savings[i].no_withdrawals++;

                file << "WITHDRAWAL SAVINGS " << amount << std::endl;
                printf("WITHDRAWAL SAVINGS %d \n", amount);
            }
            else
            {
                savings_account.no_rejected++;
                th_savings[i].no_rejected++;

                file << "WITHDRAWAL SAVINGS " << amount << " (REJECTED)" << std::endl;
                printf("WITHDRAWAL SAVINGS %d (REJECTED) \n", amount);
            }
        }
        // TRANSFER CHECKING TO SAVINGS
        else if (transaction == 5)
        {
            int amount = rand() % 200 + 100;
            if (th_checking[i].balance >= amount)
            {
                checking_account.balance -= amount;
                th_checking[i].balance -= amount;

                checking_account.no_withdrawals++;
                th_checking[i].no_withdrawals++;

                savings_account.balance += amount;
                th_savings[i].balance += amount;

                savings_account.no_deposits++;
                th_savings[i].no_deposits++;

                file << "TRANSFER CHECKING TO SAVINGS " << amount << std::endl;
                printf("TRANSFER CHECKING TO SAVINGS  %d \n", amount);
            }
            else
            {
                checking_account.no_rejected++;
                th_checking[i].no_rejected++;

                //savings_account.no_rejected++;
                //th_savings[i].no_rejected++;

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
                savings_account.balance -= amount;
                th_savings[i].balance -= amount;

                savings_account.no_withdrawals++;
                th_savings[i].no_withdrawals++;

                checking_account.balance += amount;
                th_checking[i].balance += amount;

                checking_account.no_deposits++;
                th_checking[i].no_deposits++;

                file << "TRANSFER SAVINGS TO CHECKING " << amount << std::endl;
                printf("TRANSFER SAVINGS TO CHECKING %d \n", amount);
            }
            else
            {
                savings_account.no_rejected++;
                th_savings[i].no_rejected++;

                //checking_account.no_rejected++;
                //th_checking[i].no_rejected++;

                file << "TRANSFER SAVINGS TO CHECKING " << amount << " (REJECTED)" << std::endl;
                printf("TRANSFER SAVINGS TO CHECKING %d (REJECTED) \n", amount);
            }
        }

       // To simulate what CPU instructions do for ++count
		int reg = count;
		++reg;
		usleep(rand() % 200000); // to increase probability of race condition
		count = reg;
    }

    // write final results
    file << "\nThread " << i << " stats: " << std::endl;
    file << "CHECKING BALANCE " << th_checking[i].balance << std::endl;
    file << "CHECKING DEPOSITS " << th_checking[i].no_deposits << std::endl;
    file << "CHECKING WITHDRAWALS " << th_checking[i].no_withdrawals << std::endl;
    file << "CHECKING REJECTED TRANSACTIONS " << th_checking[i].no_rejected<< std::endl;

    file << "SAVINGS BALANCE " << th_savings[i].balance << std::endl;
    file << "SAVINGS DEPOSITS " << th_savings[i].no_deposits << std::endl;
    file << "SAVINGS WITHDRAWALS " << th_savings[i].no_withdrawals << std::endl;
    file << "SAVINGS REJECTED TRANSACTIONS " << th_savings[i].no_rejected << "\n"
         << std::endl;

    printf("\nThread %d stats: \n", i);
    printf("CHECKING BALANCE %d \n", th_checking[i].balance);
    printf("CHECKING DEPOSITS %d \n", th_checking[i].no_deposits);
    printf("CHECKING WITHDRAWALS %d \n", th_checking[i].no_withdrawals);
    printf("CHECKING REJECTED TRANSACTIONS %d \n", th_checking[i].no_rejected);

    printf("SAVINGS BALANCE %d \n", th_savings[i].balance);
    printf("SAVINGS DEPOSITS %d \n", th_savings[i].no_deposits);
    printf("SAVINGS WITHDRAWALS %d \n", th_savings[i].no_withdrawals);
    printf("SAVINGS REJECTED TRANSACTIONS %d \n\n", th_savings[i].no_rejected);

    file.close();
    //exit(0); // not all threads may complete the loop due to race condition
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

    //printf("\n\n\n BEEP BOOP \n\n\n");

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

    // pass loop_count and filename to thread
    parameters *p1_params = new parameters;
    p1_params->loop_count = loop_count;
    p1_params->filename = (char *)"t1_race.txt";
    p1_params->thr_index = 1;

    parameters *p2_params = new parameters;
    p2_params->loop_count = loop_count;
    p2_params->filename = (char *)"t2_race.txt";
    p2_params->thr_index = 2;

    parameters *p3_params = new parameters;
    p3_params->loop_count = loop_count;
    p3_params->filename = (char *)"t3_race.txt";
    p3_params->thr_index = 3;

    parameters *p4_params = new parameters;
    p4_params->loop_count = loop_count;
    p4_params->filename = (char *)"t4_race.txt";
    p4_params->thr_index = 4;

    parameters *p5_params = new parameters;
    p5_params->loop_count = loop_count;
    p5_params->filename = (char *)"t5_race.txt";
    p5_params->thr_index = 5;

    parameters *p6_params = new parameters;
    p6_params->loop_count = loop_count;
    p6_params->filename = (char *)"t6_race.txt";
    p6_params->thr_index = 6;

    parameters *p7_params = new parameters;
    p7_params->loop_count = loop_count;
    p7_params->filename = (char *)"t7_race.txt";
    p7_params->thr_index = 7;

    parameters *p8_params = new parameters;
    p8_params->loop_count = loop_count;
    p8_params->filename = (char *)"t8_race.txt";
    p8_params->thr_index = 8;

    parameters *p9_params = new parameters;
    p9_params->loop_count = loop_count;
    p9_params->filename = (char *)"t9_race.txt";
    p9_params->thr_index = 9;

    parameters *p10_params = new parameters;
    p10_params->loop_count = loop_count;
    p10_params->filename = (char *)"t10_race.txt";
    p10_params->thr_index = 10;

    pthread_create(&tr1, &attr, banking, (void *) p1_params);
    pthread_create(&tr2, &attr, banking, (void *) p2_params);
    pthread_create(&tr3, &attr, banking, (void *) p3_params);
    pthread_create(&tr4, &attr, banking, (void *) p4_params);
    pthread_create(&tr5, &attr, banking, (void *) p5_params);
    pthread_create(&tr6, &attr, banking, (void *) p6_params);
    pthread_create(&tr7, &attr, banking, (void *) p7_params);
    pthread_create(&tr8, &attr, banking, (void *) p8_params);
    pthread_create(&tr9, &attr, banking, (void *) p9_params);
    pthread_create(&tr10, &attr, banking, (void *) p10_params);

    //pthread_create(&t_c1, &attr, checking, (void *)c_params);

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

    //pthread_join(t_c1, NULL);

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
        int c_sum;
        int c_deposits;
        int c_withdrawals;
        int c_rejected;

        int s_sum;
        int s_deposits;
        int s_withdrawals;
        int s_rejected;

        for(int i = 0; i<10; i++) {
            c_sum += th_checking[i].balance;
            c_deposits += th_checking[i].no_deposits;
            c_withdrawals += th_checking[i].no_withdrawals;
            c_rejected += th_checking[i].no_rejected;

            s_sum += th_savings[i].balance;
            s_deposits += th_savings[i].no_deposits;
            s_withdrawals += th_savings[i].no_withdrawals;
            s_rejected += th_savings[i].no_rejected;
        }

        //int c_diff = checking_account.balance - c_sum;
        //int s_diff = savings_account.balance - s_sum;

        printf("Shared Checking: %d\n", checking_account.balance);
        printf("Individual Sum Checking: %d\n", c_sum);
        printf("Difference: %d\n", (checking_account.balance - c_sum));

        printf("Shared Savings: %d\n", savings_account.balance);
        printf("Individual Sum Savings: %d\n", s_sum);
        printf("Difference: %d\n", (savings_account.balance - s_sum));
}
