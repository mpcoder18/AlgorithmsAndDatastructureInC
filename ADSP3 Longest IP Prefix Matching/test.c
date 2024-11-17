#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BITS 32

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
    printf("%d", current->interface);
}

// Function to search the best matching subnet in the Trie
int searchBestMatch(TrieNode* root, unsigned int ip) {
    TrieNode* current = root;
    int bestInterface = -1;
    printf("%d", current->interface);
    for (int i = 0; i < MAX_BITS-8; i++) {
        int bit = (ip >> (MAX_BITS - 1 - i)) & 1;
        //if (current->children[bit] == NULL) {
            //break;
        //}
        //current = current->children[bit];
        //if (current->interface != -1) {
            //bestInterface = current->interface;
        //}
    }
    return current->interface;
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

    // Reading subnets and inserting them into the Trie
    for (int i = 0; i < n; i++) {
        scanf("%s %d", ipStr, &interface);
        char* slashPos = strchr(ipStr, '/');
        *slashPos = '\0';  // Split IP and mask
        mask = atoi(slashPos + 1);
        unsigned int ip = ipToInt(ipStr);
        insertSubnet(root, ip, mask, interface);
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