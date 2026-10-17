#include <iostream>
#include <string>
#include <iomanip>
#include <stack>
#include <fstream>
#include <vector>

using namespace std;

static LogNode* head = NULL;
static int TestCount = 1;

const long long BASE = 257LL;
const long long MOD = 1000000007LL;

long long GetHighestPower(int k)
{
    long long power = 1;

    for (int i = 0; i < k - 1; i++)
    {
        power = (power * BASE) % MOD;
    }

    return power;
}

long long GetInitialHash(const string &text, int k)
{
    long long hashval = 0;

    for (int i = 0; i < k; i++)
    {
        hashval = (hashval * BASE + (unsigned char)text[i]) % MOD;
    }

    return hashval;
}

long long RollHash(long long oldhash, char oldc, char newc,
                   long long power)
{
    long long outval =
        ((long long)(unsigned char)oldc * power) % MOD;

    long long rem = (oldhash - outval) % MOD;

    if (rem < 0)
        rem += MOD;

    long long newhash =
        (rem * BASE + (unsigned char)newc) % MOD;

    return newhash;
}

float CalculateSimilarity(int matches, int totalKgrams)
{
    if (totalKgrams <= 0)
    {
        return 0.0f;
    }

    return ((float)matches / (float)totalKgrams) * 100.0f;
}

string RiskResult(float score)
{
    if (score < 15.0f)
    {
        return "Low Risk : Minimal Similarity";
    }
    else if (score <= 40.0f)
    {
        return "Moderate Risk : Possible Reuse";
    }
    else
    {
        return "High Risk : Strong Copy";
    }
}

int checkBrackets(const string& text)
{
    stack<char> bracketStack;

    int line = 1;
    int col = 0;
    int inLineComment = 0;
    int inBlockComment = 0;
    int inQuotes = 0;
    int inChar = 0;

    for (int i = 0; i < text.length(); i++)
    {
        char ch = text[i];
        col++;

        if (ch == '\n')
        {
            line++;
            col = 0;
            inLineComment = 0;
            continue;
        }

        if (inLineComment == 1)
            continue;

        if (inBlockComment == 1)
        {
            if (ch == '*' && i + 1 < text.length()
                && text[i + 1] == '/')
            {
                inBlockComment = 0;
                i++;
                col++;
            }
            continue;
        }

        if (inQuotes == 1)
        {
            if (ch == '\\' && i + 1 < text.length())
            {
                i++;
                col++;
                continue;
            }

            if (ch == '"')
                inQuotes = 0;

            continue;
        }

        if (inChar == 1)
        {
            if (ch == '\\' && i + 1 < text.length())
            {
                i++;
                col++;
                continue;
            }

            if (ch == '\'')
                inChar = 0;

            continue;
        }

        if (ch == '/' && i + 1 < text.length()
            && text[i + 1] == '/')
        {
            inLineComment = 1;
            i++;
            col++;
            continue;
        }

        if (ch == '/' && i + 1 < text.length()
            && text[i + 1] == '*')
        {
            inBlockComment = 1;
            i++;
            col++;
            continue;
        }

        if (ch == '"')
        {
            inQuotes = 1;
            continue;
        }

        if (ch == '\'')
        {
            inChar = 1;
            continue;
        }

        if (ch == '(' || ch == '{' || ch == '[')
        {
            bracketStack.push(ch);
        }
        else if (ch == ')' || ch == '}' || ch == ']')
        {
            if (bracketStack.empty())
            {
                cout << "Closing bracket without opening bracket: "
                     << ch << " at Line " << line
                     << ", Col " << col << "\n";

                return 0;
            }

            char topBracket = bracketStack.top();
            bracketStack.pop();

            if ((ch == ')' && topBracket != '(') ||
                (ch == '}' && topBracket != '{') ||
                (ch == ']' && topBracket != '['))
            {
                cout << "Brackets do not match: expected match for '"
                     << topBracket << "', but found '" << ch
                     << "' at Line " << line
                     << ", Col " << col << "\n";

                return 0;
            }
        }
    }

    if (!bracketStack.empty())
    {
        cout << "Unclosed bracket remaining on stack: '"
             << bracketStack.top() << "'\n";

        return 0;
    }

    cout << "All brackets are balanced!\n";
    return 1;
}

char toLowerChar(char ch)
{
    if (ch >= 'A' && ch <= 'Z')
    {
        return ch + 32;
    }

    return ch;
}

bool isAlnumChar(char ch)
{
    if (ch >= 'a' && ch <= 'z')
        return true;
    else if (ch >= 'A' && ch <= 'Z')
        return true;
    else if (ch >= '0' && ch <= '9')
        return true;
    else
        return false;
}

struct TokenNode
{
    string value;
    bool isIdentifier;
    TokenNode* prev;
    TokenNode* next;

    TokenNode(const string& val, bool isIdent)
    {
        value = val;
        isIdentifier = isIdent;
        prev = NULL;
        next = NULL;
    }
};

class TokenList
{
private:
    TokenNode* head;
    TokenNode* tail;

public:
    TokenList()
    {
        head = NULL;
        tail = NULL;
    }

    ~TokenList()
    {
        TokenNode* curr = head;

        while (curr != NULL)
        {
            TokenNode* nextNode = curr->next;
            delete curr;
            curr = nextNode;
        }
    }

    void append(const string& val, bool isIdent)
    {
        TokenNode* newNode = new TokenNode(val, isIdent);

        if (head == NULL)
        {
            head = tail = newNode;
        }
        else
        {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
    }

    int replaceAll(const string& oldWord, const string& newWord)
    {
        int replacements = 0;
        TokenNode* curr = head;

        while (curr != NULL)
        {
            if (curr->isIdentifier && curr->value == oldWord)
            {
                curr->value = newWord;
                replacements++;
            }

            curr = curr->next;
        }

        return replacements;
    }

    string reconstruct() const
    {
        string result = "";
        TokenNode* curr = head;

        while (curr != NULL)
        {
            result += curr->value;
            curr = curr->next;
        }

        return result;
    }
};

void replaceWord(string& text, const string& oldWord,
                 const string& newWord)
{
    if (oldWord.empty())
        return;

    TokenList tokenList;

    int i = 0;
    int len = text.length();

    while (i < len)
    {
        if (isAlnumChar(text[i]) || text[i] == '_')
        {
            int start = i;

            while (i < len &&
                   (isAlnumChar(text[i]) || text[i] == '_'))
            {
                i++;
            }

            tokenList.append(text.substr(start, i - start), true);
        }
        else
        {
            int start = i;

            while (i < len &&
                   !(isAlnumChar(text[i]) || text[i] == '_'))
            {
                i++;
            }

            tokenList.append(text.substr(start, i - start), false);
        }
    }

    int count = tokenList.replaceAll(oldWord, newWord);
    text = tokenList.reconstruct();

    cout << "Replaced " << count << " occurrence(s) of '"
         << oldWord << "' with '" << newWord << "'.\n";
}

struct ScopeNode
{
    char brace;
    int index;
    ScopeNode* next;

    ScopeNode(char b, int idx, ScopeNode* nxt = NULL)
    {
        brace = b;
        index = idx;
        next = nxt;
    }
};

class ScopeStack
{
private:
    ScopeNode* topNode;

public:
    ScopeStack()
    {
        topNode = NULL;
    }

    ~ScopeStack()
    {
        while (!isEmpty())
        {
            pop();
        }
    }

    void push(char brace, int idx)
    {
        topNode = new ScopeNode(brace, idx, topNode);
    }

    bool pop()
    {
        if (isEmpty())
            return false;

        ScopeNode* temp = topNode;
        topNode = topNode->next;
        delete temp;

        return true;
    }

    bool isEmpty() const
    {
        return topNode == NULL;
    }
};

int SimpleHashTable::getBucketIndex(long long hash)
{
    int index = hash % BUCKET_COUNT;

    if (index < 0)
        index += BUCKET_COUNT;

    return index;
}

SimpleHashTable::SimpleHashTable()
    : buckets(BUCKET_COUNT)
{
}

void SimpleHashTable::insert(long long hash, const string &chunk)
{
    int index = getBucketIndex(hash);

    for (int i = 0; i < buckets[index].size(); i++)
    {
        if (buckets[index][i].hashVal == hash)
        {
            for (int j = 0;
                 j < buckets[index][i].strings.size(); j++)
            {
                if (buckets[index][i].strings[j] == chunk)
                    return;
            }

            buckets[index][i].strings.push_back(chunk);
            return;
        }
    }

    HashBucketItem item;
    item.hashVal = hash;
    item.strings.push_back(chunk);

    buckets[index].push_back(item);
}

const HashBucketItem *SimpleHashTable::find(long long hash) const
{
    int index = getBucketIndex(hash);

    for (int i = 0; i < buckets[index].size(); i++)
    {
        if (buckets[index][i].hashVal == hash)
            return &buckets[index][i];
    }

    return NULL;
}

int main()
{
    string file1;
    string file2;
    int k = 10;

    loadSamples(file1, file2);

    int choice = 0;

    while (true)
    {
        cout << "1. Run Plagiarism Evaluation\n";
        cout << "2. Check Balanced Brackets\n";
        cout << "3. Edit File 1 (Undo/Redo)\n";
        cout << "4. Rename Variable / Identifier in File 2\n";
        cout << "5. Extract Function for Specific Comparison\n";
        cout << "6. View Cleaned Code (Comments & Spaces Removed)\n";
        cout << "7. View Session Evaluation History\n";
        cout << "8. Change k-gram Window Size (Current: "
             << k << ")\n";
        cout << "9. Load Files (.c / .cpp)\n";
        cout << "10. Exit\n";
        cout << "Enter your choice (1-10): ";

        if (!(cin >> choice))
        {
            if (cin.eof())
                break;

            cin.clear();
            flushInputLine();
            continue;
        }

        flushInputLine();
    }

    return 0;
}
