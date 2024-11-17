#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BITS 32


void octetToBinaryString(int octet, char *output) {
    for (int i = 7; i >= 0; i--) {
        output[7 - i] = (octet & (1 << i)) ? '1' : '0';
    }
    output[8] = '\0';  // Null-terminate the string
}

void ipToBinaryString(const char* ipAddress, char* binaryString) {
    int octets[4];
    char binaryOctet[9];  // 8 bits + null terminator
    
    // Parse the IP address into four octets
    sscanf(ipAddress, "%d.%d.%d.%d", &octets[0], &octets[1], &octets[2], &octets[3]);

    // Convert each octet to binary and concatenate to the final string
    binaryString[0] = '\0';  // Initialize the binaryString as an empty string

    for (int i = 0; i < 4; i++) {
        octetToBinaryString(octets[i], binaryOctet);
        strcat(binaryString, binaryOctet);
        if (i < 3) {
            strcat(binaryString, ".");  // Add a dot between octets
        }
    }
}

void parseIpAndSubnet(const char* input, char* ipAddress, int* subnetMask) {
    char inputCopy[20];
    strcpy(inputCopy, input);  // Make a copy of the input to modify
    
    // Use strtok to split the IP address and the subnet mask
    char* token = strtok(inputCopy, "/");
    if (token != NULL) {
        strcpy(ipAddress, token);  // Copy the IP address part
        
        // Get the subnet mask part
        token = strtok(NULL, "/");
        if (token != NULL) {
            *subnetMask = strtol(token, NULL, 10);  // Convert subnet mask to integer
        }
    }
}


// Trie node structure
typedef struct TrieNode {
    int interface;   // Store the interface number if this node is the end of a subnet
    struct TrieNode* children[2];  // Two children: 0 and 1
} TrieNode;

// Function to create a new Trie node
TrieNode* createTrieNode() {
    TrieNode* node = (TrieNode*)malloc(sizeof(TrieNode));
    node->interface = -1;  // -1 means no interface assigned yet
    node->children[0] = node->children[1] = NULL;
    return node;
}

// Function to insert a subnet into the Trie
void insertSubnet(TrieNode* root, unsigned int ip, int mask, int interface) {
    TrieNode* current = root;
    
    for (int i = MAX_BITS - mask; i < MAX_BITS; i++) {
        int bit = (ip >> (MAX_BITS - 1 - i)) & 1;
        if (current->children[bit] == NULL) {
            current->children[bit] = createTrieNode();
        }
        current = current->children[bit];
    }
    current->interface = interface;
}

// Function to search the best matching subnet in the Trie
int searchBestMatch(TrieNode* root, unsigned int ip) {
    TrieNode* current = root;
    int bestInterface = -1;
    for (int i = 0; i < MAX_BITS; i++) {
        int bit = (ip >> (MAX_BITS - 1 - i)) & 1;
        if (current->children[bit] == NULL) {
            break;
        }
        current = current->children[bit];
        if (current->interface != -1) {
            bestInterface = current->interface;
        }
    }
    return bestInterface;
}

// Convert an IP address string to a 32-bit integer
unsigned int ipToInt(const char* ipStr) {
    unsigned int ip = 0;
    int octet;
    sscanf(ipStr, "%d", &octet);
    ip |= (octet << 24);
    sscanf(strchr(ipStr, '.') + 1, "%d", &octet);
    ip |= (octet << 16);
    sscanf(strchr(strchr(ipStr, '.') + 1, '.') + 1, "%d", &octet);
    ip |= (octet << 8);
    sscanf(strrchr(ipStr, '.') + 1, "%d", &octet);
    ip |= octet;
    return ip;
}

int main() {
    int n, m;
    char ipStr[20];
    int mask, interface;
    
    TrieNode* root = createTrieNode();

    // Reading the number of subnets
    scanf("%d", &n);

char ipAddress[16];
char binaryString[33];  // 32 bits + 3 dots + null terminator
    // Reading subnets and inserting them into the Trie
    for (int i = 0; i < n; i++) {
        scanf("%s %d", ipStr, &interface);
        printf("%d", interface);
        parseIpAndSubnet(ipStr, ipAddress, &mask);
    ipToBinaryString(ipAddress, binaryString);
    // ik wil van de binary string wil ik 

    printf("IP Address: %s\n", ipAddress);
    printf("Binary String: %s\n", binaryString);



        //insertSubnet(root, ip, mask, interface);
    }

    // Reading the number of IP addresses to check
    scanf("%d", &m);

    // Processing each IP and finding the best matching subnet
    for (int i = 0; i < m; i++) {
        scanf("%s", ipStr);
        unsigned int ip = ipToInt(ipStr);
        int bestMatchInterface = searchBestMatch(root, ip);
        printf("%d\n", bestMatchInterface);
    }

    return 0;
}

/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


typedef struct TreeNode *Tree;
struct TreeNode {
int item;
int interface;
Tree leftChild, rightChild;
};

Tree emptyTree() {
return NULL;
}


Tree newTree(int n, Tree tL, Tree tR) {
Tree new = malloc(sizeof(struct TreeNode));
assert(new != NULL);
new->item = n;
new->leftChild = tL;
new->rightChild = tR;
return new;
}



Tree searchNode(Tree t, int n) {
if (t == NULL) {
return NULL;
}
if (n == t->item) {
return t;
}
if ( n < t->item ) {
t = t->leftChild;
} else {
t = t->rightChild;
}
return searchNode(t,n);
}

Tree addInSearchTree(Tree t, int n) {
if (t == NULL) {
return newTree(n,emptyTree(), emptyTree());
}
if (n < t->item){
t->leftChild = addInSearchTree(t->leftChild,n);
} else if (n > t->item) {
t->rightChild = addInSearchTree(t->rightChild,n);
}
return t;
}








int main(){
// ik wil natuurlijk eerst de dingen lezne 






    return 0;
}*/
