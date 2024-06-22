
#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <getopt.h>
#include <queue>
#include <cctype>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <algorithm>

using namespace std;

struct transaction
{
public:
    // timestamp,IP,proposed execution date, and whether the transaction fee is covered by our account
    string timestamp;
    string senderIP;
    string senderID;
    string recipientID;
    uint32_t amount;
    string execDate;
    string feeCoverage;
    uint32_t id;
    uint32_t fee;
};

struct user
{
public:
    // here do reg tim
    // reg timestamp
    // where the user is logged in and what they've done
    string id;
    string pin;
    string ip_address;
    bool sessionActive = false;
    uint32_t numSessions;
    unordered_set<string> ip_list;
    uint32_t balance;
    uint64_t date;
    vector<transaction> incoming;
    vector<transaction> outgoing;
};

struct transactionCompare
{
public:
    bool operator()(const transaction &a, const transaction &b) const
    {
        if (a.execDate != b.execDate)
        {
            return a.execDate > b.execDate;
        }
        else
        {
            return a.id > b.id;
        }
    }
};

class bank_solver
{
public:
    bool isFile = false;
    bool isVerbose = false;

    // variable for reg timestamp
    string reg_name;
    string reg_timestamp;
    string reg_id;
    string reg_pin;
    uint32_t starting_balance;
    uint32_t id_counter = 0;

    // unordered map for ip addreses

    unordered_map<string, user> userBalances;
    priority_queue<transaction, vector<transaction>, transactionCompare> transaction_list;
    vector<transaction> masterTransaction;
    // do something with lower bound here

    void executeTransactions()
    {
        transaction transactionOne = transaction_list.top();

        auto sender = userBalances.find(transactionOne.senderID);
        auto recipient = userBalances.find(transactionOne.recipientID);

        uint32_t feeSender = 0;
        uint32_t feeRecipient = 0;

        if (transactionOne.feeCoverage == "s")
        {
            feeSender = transactionOne.fee / 2;
            feeRecipient = transactionOne.fee / 2;
            if (transactionOne.fee % 2 == 1)
            {
                feeSender++;
            }
            if (sender->second.balance >= transactionOne.amount + feeSender &&
                recipient->second.balance >= feeRecipient)
            {

                sender->second.balance -= transactionOne.amount;
                sender->second.balance -= feeSender;
                recipient->second.balance += transactionOne.amount;
                recipient->second.balance -= feeRecipient;

                uint64_t nonZero3 = stoull(transactionOne.execDate);

                if (isVerbose)
                {
                    cout << "Transaction executed at " << nonZero3 << ": $"
                         << transactionOne.amount << " from " << transactionOne.senderID
                         << " to " << transactionOne.recipientID << "." << endl;
                }
                masterTransaction.push_back(transactionOne);
                sender->second.outgoing.push_back(transactionOne);
                recipient->second.incoming.push_back(transactionOne);
                transaction_list.pop();
            }
            else
            {
                if (isVerbose)
                {
                    cout << "Insufficient funds to process transaction " << transactionOne.id << "." << endl;
                }
                transaction_list.pop();
            }
        }
        else if (transactionOne.feeCoverage == "o")
        {
            // feeSender = transactionOne.amount / 2;
            feeSender = transactionOne.fee;

            if (sender->second.balance >= transactionOne.amount + feeSender)
            {

                sender->second.balance -= transactionOne.amount + feeSender;
                recipient->second.balance += transactionOne.amount;

                uint64_t nonZero3 = stoull(transactionOne.execDate);

                if (isVerbose)
                {
                    cout << "Transaction executed at " << nonZero3 << ": $"
                         << transactionOne.amount << " from " << transactionOne.senderID
                         << " to " << transactionOne.recipientID << "." << endl;
                }
                masterTransaction.push_back(transactionOne);
                sender->second.outgoing.push_back(transactionOne);
                recipient->second.incoming.push_back(transactionOne);
                transaction_list.pop();
            }
            else
            {
                if (isVerbose)
                {
                    cout << "Insufficient funds to process transaction " << transactionOne.id << "." << endl;
                }
                transaction_list.pop();
            }
        }
    }

    void
    getMode(int argc, char *argv[])
    {
        // These are used with getopt_long()
        opterr = false; // Let us handle all error output for command line options
        int choice;
        int index = 0;

        option long_options[] = {
            // TODO: Fill in two lines, for the "mode" ('m') and
            // the "help" ('h') options.
            {"help", no_argument, nullptr, 'h'},
            {"file", required_argument, nullptr, 'f'},
            {"verbose", no_argument, nullptr, 'v'},
            {nullptr, 0, nullptr, '\0'}}; // long_options[]

        // TODO: Fill in the double quotes, to match the mode and help options.
        string arg;
        while ((choice = getopt_long(argc, argv, "hf:v", long_options, &index)) != -1)
        {
            switch (choice)
            {
            case 'h':
                cout << "This is a bank wire transfer simulator developed by EECS 281." << endl;
                exit(0);

            case 'f':
                arg = optarg;
                reg_name = arg;
                break;

            case 'v':
                isVerbose = true;
                break;

            default:

                cerr << "Error: invalid option" << endl;
                exit(1);
                // switch ..choice
            } // while
            // if ..mode  // getMode()
        }
    }

    void userCommands()
    {
        // human human1;
        // human1.pin = 66;
        // ip_list[human1] = 4;
        // cout << ip_list[]
        // use .find
        // use .substr

        string line;

        ifstream fin(reg_name);

        string reg_timestamp, reg_id, reg_pin;
        uint32_t starting_balance;
        while (getline(fin, reg_timestamp, '|') && getline(fin, reg_id, '|') && getline(fin, reg_pin, '|') && fin >> starting_balance)
        {

            user regUser;

            regUser.balance = starting_balance;
            reg_timestamp.erase(std::remove(reg_timestamp.begin(), reg_timestamp.end(), ':'), reg_timestamp.end());
            // Assuming date is a uint64_t
            regUser.date = stoull(reg_timestamp);
            regUser.pin = reg_pin;
            regUser.id = reg_id;
            regUser.numSessions = 0;

            userBalances[reg_id] = regUser; // Store the user struct in the map
        }

        while (cin >> line)
        {
            if (cin.fail())
            {
                cerr << "Error: Reading from cin has failed" << endl;
                exit(1);
            } // if
            else if (line[0] == '$' && line[1] == '$' && line[2] == '$')
            {
                while (!transaction_list.empty())
                {
                    executeTransactions();
                }
                query_list();
            }

            else if (line[0] == '#')
            {
                string junk;
                getline(cin, junk);
            }
            else if (line[0] == 'l')
            {

                string currUserId, currUserIP;
                string currUserPin;
                cin >> currUserId >> currUserPin >> currUserIP;
                // error on the following line
                auto finder = userBalances.find(currUserId);
                if (finder == userBalances.end())
                {
                    if (isVerbose)
                    {
                        cout << "Failed to log in " << currUserId << "." << endl;
                    }
                }
                else if (finder->second.pin == currUserPin && finder != userBalances.end())
                {
                    // Directly modify the user in the map
                    finder->second.sessionActive = true;
                    finder->second.ip_list.insert(currUserIP);

                    if (isVerbose)
                    {
                        cout << "User " << currUserId << " logged in." << endl;
                    }
                }

                else
                {
                    if (isVerbose)
                    {
                        cout << "Failed to log in " << currUserId << "." << endl;
                    }
                }
            }
            else if (line[0] == 'o')
            {
                string currUserId, currUserIP;
                cin >> currUserId >> currUserIP;
                auto finder = userBalances.find(currUserId);
                if (finder == userBalances.end())
                {
                    if (isVerbose)
                    {
                        cout << "Failed to log out " << currUserId << "." << endl;
                    }
                }
                else if (finder->second.sessionActive && finder->second.ip_list.find(currUserIP) != finder->second.ip_list.end())

                {

                    finder->second.ip_list.erase(currUserIP);
                    if (finder->second.ip_list.size() == 0)
                    {
                        finder->second.sessionActive = false;
                    }
                    if (isVerbose)
                    {
                        cout << "User " << currUserId << " logged out." << endl;
                    }
                }
                else
                {
                    if (isVerbose)
                    {
                        cout << "Failed to log out " << currUserId << "." << endl;
                    }
                }
            }
            else if (line[0] == 'p')
            {
                transaction currTransaction;

                string currTimestamp, IP, senderID, recipientID, execDate, feeCoverage;
                uint32_t amount;
                cin >> currTimestamp >> IP >> senderID >> recipientID >> amount >> execDate >> feeCoverage;
                //******* ERROR CHECKING *******
                currTimestamp.erase(std::remove(currTimestamp.begin(), currTimestamp.end(), ':'), currTimestamp.end());
                uint64_t timestamp = stoull(currTimestamp);
                execDate.erase(std::remove(execDate.begin(), execDate.end(), ':'), execDate.end());
                uint64_t date = stoull(execDate);

                auto findSender = userBalances.find(senderID);
                auto findRecipient = userBalances.find(recipientID);

                if (date - timestamp > 3000000)
                {
                    if (isVerbose)
                    {
                        cout << "Select a time less than three days in the future." << endl;
                    }
                }

                else if (findSender == userBalances.end())
                {
                    if (isVerbose)
                    {
                        cout << "Sender " << senderID << " does not exist." << endl;
                    }
                }
                else if (findRecipient == userBalances.end())
                {
                    if (isVerbose)
                    {
                        cout << "Recipient " << recipientID << " does not exist." << endl;
                    }
                }
                else if (date <= (findSender->second.date) || date <= (findRecipient->second.date)) /*isnt later than both registration dates */
                {
                    if (isVerbose)
                    {
                        cout << "At the time of execution, sender and/or recipient have not registered." << endl;
                    }
                }
                else if (!findSender->second.sessionActive)
                {
                    if (isVerbose)
                    {
                        cout << "Sender " << senderID << " is not logged in." << endl;
                    }
                }
                // checking fraudulent transaction, this may be incorrect .
                // first condition may not be necessary, fix if having errors.
                else if (findSender->second.ip_list.find(IP) == findSender->second.ip_list.end())
                {
                    if (isVerbose)
                    {
                        cout << "Fraudulent transaction detected, aborting request." << endl;
                    }
                }
                //**ERROR CHECKING ENDS HERE*******
                else
                {

                    // Within the execution logic
                    // may get bad access error here, fix logic if that happens

                    while (!transaction_list.empty() && timestamp >= stoull(transaction_list.top().execDate))
                    {

                        // uint64_t currentTimestamp = stoulll(currTransaction.timestamp);
                        executeTransactions();
                        // getline(regFile, str, '|');
                    }

                    currTransaction.timestamp = currTimestamp;
                    currTransaction.senderIP = IP;
                    currTransaction.senderID = senderID;
                    currTransaction.recipientID = recipientID;
                    currTransaction.amount = amount;
                    currTransaction.execDate = execDate;
                    currTransaction.feeCoverage = feeCoverage;
                    currTransaction.fee = static_cast<uint32_t>(max(10.0, min(450.0, amount / 100.0)));

                    currTransaction.id = (id_counter++);
                    uint64_t newDate = stoull(currTransaction.execDate);

                    if (newDate - (userBalances[senderID].date) > 50000000000ULL)
                    {
                        currTransaction.fee = (currTransaction.fee * 3) / 4;
                    }

                    // findSender->second.balance -= currTransaction.amount + currTransaction.fee;
                    transaction_list.push(currTransaction);

                    uint64_t nonZero1 = stoull(currTransaction.timestamp);
                    uint64_t nonZero2 = stoull(currTransaction.execDate);
                    if (isVerbose)
                    {
                        cout << "Transaction placed at " << nonZero1 << ": $"
                             << currTransaction.amount << " from " << currTransaction.senderID
                             << " to " << currTransaction.recipientID << " at " << nonZero2 << "." << endl;
                    }

                    // Do not pop from transaction_list here, it's handled in the execution loop

                    // Transaction Execution

                    // if ()
                }
            }
        }
    }
    void query_list()
    {
        string s;

        while (cin >> s)
        {
            if (s[0] == 'l')
            {
                string x, y;
                uint32_t transactionCount = 0;
                cin >> x >> y;
                x.erase(std::remove(x.begin(), x.end(), ':'), x.end());
                y.erase(std::remove(y.begin(), y.end(), ':'), y.end());
                uint64_t newX = stoull(x);
                uint64_t newY = stoull(y);

                auto start = lower_bound(masterTransaction.begin(), masterTransaction.end(), newX, [](const transaction &i, uint64_t j)
                                         { return stoull(i.execDate) < j; });

                auto end = lower_bound(masterTransaction.begin(), masterTransaction.end(), newY, [](const transaction &i, uint64_t j)
                                       { return stoull(i.execDate) < j; });
                for (auto i1 = start; i1 != end; i1++)
                {
                    if (i1->amount == 1)
                    {
                        cout << i1->id << ": " << i1->senderID << " sent " << i1->amount << " dollar to " << i1->recipientID << " at " << stoull(i1->execDate) << ".\n";
                    }
                    else
                    {
                        cout << i1->id << ": " << i1->senderID << " sent " << i1->amount << " dollars to " << i1->recipientID << " at " << stoull(i1->execDate) << ".\n";
                    }
                    transactionCount++;
                }
                if (transactionCount == 1)
                {
                    cout << "There was 1 transaction that was placed between time " << newX << " to " << newY << ".\n";
                }
                else
                {
                    cout << "There were " << transactionCount << " transactions that were placed between time " << newX << " to " << newY << ".\n";
                }
            }
            else if (s[0] == 'r')
            {
                string x, y;
                uint32_t transactionAmount = 0;
                cin >> x >> y;
                x.erase(std::remove(x.begin(), x.end(), ':'), x.end());
                y.erase(std::remove(y.begin(), y.end(), ':'), y.end());
                uint64_t newX = stoull(x);
                uint64_t newY = stoull(y);
                uint64_t difference = newY - newX;
                uint64_t seconds = difference % 100ull;
                uint64_t minutes = difference % 10000ull / 100ull;
                uint64_t hour = difference % 1000000ull / 10000ull;
                uint64_t day = difference % 100000000ull / 1000000ull;
                uint64_t month = difference % 10000000000ull / 100000000ull;
                uint64_t year = (difference) % 1000000000000ull / 10000000000ull;

                auto start = lower_bound(masterTransaction.begin(), masterTransaction.end(), newX, [](const transaction &i, uint64_t j)
                                         { return stoull(i.execDate) < j; });

                auto end = lower_bound(masterTransaction.begin(), masterTransaction.end(), newY, [](const transaction &i, uint64_t j)
                                       { return stoull(i.execDate) < j; });
                for (auto i1 = start; i1 != end; i1++)
                {
                    transactionAmount += i1->fee;
                }
                cout << "281Bank has collected " << transactionAmount << " dollars in fees over";
                if (year > 0)
                {
                    if (year == 1)
                    {
                        cout << " " << year << " year";
                    }
                    else
                    {
                        cout << " " << year << " years";
                    }
                }

                if (month > 0)
                {
                    if (month == 1)
                    {
                        cout << " " << month << " month";
                    }
                    else
                    {
                        cout << " " << month << " months";
                    }
                }

                if (day > 0)
                {
                    if (day == 1)
                    {
                        cout << " " << day << " day";
                    }
                    else
                    {
                        cout << " " << day << " days";
                    }
                }

                if (hour > 0)
                {
                    if (hour == 1)
                    {
                        cout << " " << hour << " hour";
                    }
                    else
                    {
                        cout << " " << hour << " hours";
                    }
                }

                if (minutes > 0)
                {
                    if (minutes == 1)
                    {
                        cout << " " << minutes << " minute";
                    }
                    else
                    {
                        cout << " " << minutes << " minutes";
                    }
                }

                if (seconds > 0)
                {
                    if (seconds == 1)
                    {
                        cout << " " << seconds << " second";
                    }
                    else
                    {
                        cout << " " << seconds << " seconds";
                    }
                }
                cout << ".\n";
            }
            else if (s[0] == 'h')
            {
                string user_id;
                cin >> user_id;

                auto finder = userBalances.find(user_id);
                if (finder != userBalances.end())
                {
                    cout << "Customer " << user_id << " account summary:\n";
                    cout << "Balance: $" << finder->second.balance << "\n";
                    cout << "Total # of transactions: " << finder->second.incoming.size() + finder->second.outgoing.size() << "\n";
                    cout << "Incoming " << finder->second.incoming.size() << ":\n";

                    if (finder->second.incoming.size() > 10)
                    {
                        finder->second.incoming.end() - 10;
                    }
                    else
                    {
                        finder->second.incoming.begin();
                    }
                    uint64_t startIn = 0;
                    uint64_t startOut = 0;
                    if (finder->second.incoming.size() > 10)
                    {
                        startIn = finder->second.incoming.size() - 10;
                    }
                    for (auto it = startIn; it < finder->second.incoming.size(); ++it)
                    {
                        if (finder->second.incoming.at(it).amount == 1)
                        {
                            cout << finder->second.incoming.at(it).id << ": " << finder->second.incoming.at(it).senderID << " sent " << finder->second.incoming.at(it).amount << " dollar to " << finder->second.incoming.at(it).recipientID << " at " << stoull(finder->second.incoming.at(it).execDate) << ".\n";
                        }
                        else
                        {
                            cout << finder->second.incoming.at(it).id << ": " << finder->second.incoming.at(it).senderID << " sent " << finder->second.incoming.at(it).amount << " dollars to " << finder->second.incoming.at(it).recipientID << " at " << stoull(finder->second.incoming.at(it).execDate) << ".\n";
                        }
                    }

                    cout << "Outgoing " << finder->second.outgoing.size() << ":\n";
                    if (finder->second.outgoing.size() > 10)
                    {
                        startOut = finder->second.outgoing.size() - 10;
                    }
                    for (auto it = startOut; it < finder->second.outgoing.size(); ++it)
                    {
                        if (finder->second.outgoing.at(it).amount == 1)
                        {
                            cout << finder->second.outgoing.at(it).id << ": " << finder->second.outgoing.at(it).senderID << " sent " << finder->second.outgoing.at(it).amount << " dollar to " << finder->second.outgoing.at(it).recipientID << " at " << stoull(finder->second.outgoing.at(it).execDate) << ".\n";
                        }
                        else
                        {
                            cout << finder->second.outgoing.at(it).id << ": " << finder->second.outgoing.at(it).senderID << " sent " << finder->second.outgoing.at(it).amount << " dollars to " << finder->second.outgoing.at(it).recipientID << " at " << stoull(finder->second.outgoing.at(it).execDate) << ".\n";
                        }
                    }
                }
                else
                {
                    cout << "User " << user_id << " does not exist.\n";
                }
            }
            else if (s[0] == 's')
            {
                string timestamp;
                cin >> timestamp;
                timestamp.erase(std::remove(timestamp.begin(), timestamp.end(), ':'), timestamp.end());
                uint64_t normal = stoull(timestamp) / 1000000;
                normal *= 1000000;
                uint64_t normalOne = normal + 1000000ULL;
                uint64_t numTransactions = 0;
                uint64_t bankFees = 0;
                auto start = lower_bound(masterTransaction.begin(), masterTransaction.end(), normal, [](const transaction &i, uint64_t j)
                                         { return stoull(i.execDate) < j; });

                auto end = lower_bound(masterTransaction.begin(), masterTransaction.end(), normalOne, [](const transaction &i, uint64_t j)
                                       { return stoull(i.execDate) < j; });
                cout << "Summary of "
                     << "[" << normal << ", " << normalOne << "):\n";
                for (auto i1 = start; i1 != end; i1++)
                {
                    if (i1->amount == 1)
                    {
                        cout << i1->id << ": " << i1->senderID << " sent 1 dollar to " << i1->recipientID << " at " << stoull(i1->execDate) << ".\n";
                    }
                    else
                    {
                        cout << i1->id << ": " << i1->senderID << " sent " << i1->amount << " dollars to " << i1->recipientID << " at " << stoull(i1->execDate) << ".\n";
                    }
                    numTransactions++;
                    bankFees += i1->fee;
                }

                if (numTransactions == 1)
                {
                    cout << "There was a total of 1 transaction"
                         << ", 281Bank has collected " << bankFees << " dollars in fees.\n";
                }
                else
                {
                    cout << "There were a total of " << numTransactions << " transactions, 281Bank has collected " << bankFees << " dollars in fees.\n";
                }
            }
        }
    }
};

int main(int argc, char *argv[])
{
    ios_base::sync_with_stdio(false);
    bank_solver solver;
    solver.getMode(argc, argv);
    solver.userCommands();
}