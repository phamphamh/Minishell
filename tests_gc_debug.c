#include "includes/header.h"
#include <stdio.h>

// Fonction de débogage pour compter les allocations
static int alloc_count = 0;
static int free_count = 0;

void *debug_malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr) {
        alloc_count++;
        printf("MALLOC #%d: %p (taille: %zu)\n", alloc_count, ptr, size);
    }
    return ptr;
}

void debug_free(void *ptr) {
    if (ptr) {
        free_count++;
        printf("FREE #%d: %p\n", free_count, ptr);
        free(ptr);
    }
}

// Création d'un nœud GC sans compter comme allocation
t_gc_node *create_gc_node(void *ptr) {
    t_gc_node *node = malloc(sizeof(t_gc_node));
    if (node) {
        printf("Création nœud GC: %p pour pointeur: %p\n", node, ptr);
        node->ptr = ptr;
        node->next = NULL;
    }
    return node;
}

// Libération d'un nœud GC sans compter comme libération
void free_gc_node(t_gc_node *node) {
    if (node) {
        printf("Libération nœud GC: %p\n", node);
        free(node);
    }
}

// Version simplifiée de ft_gc_add pour déboguer
bool debug_gc_add(t_gc_node **gc_head, void *ptr) {
    t_gc_node *new_node;
    t_gc_node *current;

    if (!ptr || !gc_head)
        return (false);

    current = *gc_head;
    while (current) {
        if (current->ptr == ptr) {
            printf("Pointeur déjà dans le GC: %p\n", ptr);
            return (true);
        }
        current = current->next;
    }

    new_node = create_gc_node(ptr);
    if (!new_node)
        return (false);

    new_node->next = *gc_head;
    *gc_head = new_node;
    printf("Ajouté au GC: %p\n", ptr);
    return (true);
}

void test_multiple_references() {
    t_gc_node *gc = NULL;

    // Utiliser debug_malloc au lieu de strdup
    char *shared_str = debug_malloc(12); // "Hello World" + '\0'
    sprintf(shared_str, "Hello World");

    printf("=== Test références multiples ===\n");

    // Ajouter shared_str au GC une seule fois
    printf("Ajout de shared_str au GC\n");
    debug_gc_add(&gc, shared_str);

    // Créer les tableaux avec debug_malloc
    char **arr1 = debug_malloc(sizeof(char *) * 2);
    char **arr2 = debug_malloc(sizeof(char *) * 2);

    // Ajouter les tableaux au GC
    printf("Ajout de arr1 au GC\n");
    debug_gc_add(&gc, arr1);

    printf("Ajout de arr2 au GC\n");
    debug_gc_add(&gc, arr2);

    // Configurer le partage du même pointeur
    arr1[0] = shared_str;
    arr1[1] = NULL;

    arr2[0] = shared_str;
    arr2[1] = NULL;

    printf("✅ Allocation avec références multiples réussie\n");

    // Nettoyage du GC
    printf("Nettoyage du GC...\n");

    // Version déboguée de ft_gc_clear
    t_gc_node *current = gc;
    t_gc_node *next;

    while (current) {
        next = current->next;
        if (current->ptr) {
            printf("GC va libérer: %p\n", current->ptr);
            debug_free(current->ptr);
        }
        free_gc_node(current);
        current = next;
    }
    gc = NULL;

    printf("✅ Nettoyage du GC réussi\n");
    printf("\n=== Statistiques mémoire ===\n");
    printf("Total allocations: %d\n", alloc_count);
    printf("Total libérations: %d\n", free_count);
    if (alloc_count != free_count) {
        printf("❌ Problème de mémoire détecté! (%d allocations non libérées)\n",
               alloc_count - free_count);
    } else {
        printf("✅ Gestion de la mémoire OK.\n");
    }
}

int main() {
    test_multiple_references();
    return 0;
}