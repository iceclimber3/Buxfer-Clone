#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lists.h"

void sort_users(Group *group, User *previous, const char *user_name);
void insert_user(Group *group, User *current, User *user);

/* Add a group with name group_name to the group_list referred to by
* group_list_ptr. The groups are ordered by the time that the group was
* added to the list with new groups added to the end of the list.
*
* Returns 0 on success and -1 if a group with this name already exists.
*
* (I.e, allocate and initialize a Group struct, and insert it
* into the group_list. Note that the head of the group list might change
* which is why the first argument is a double pointer.)
*/
int add_group(Group **group_list_ptr, const char *group_name) {
    // Create a new group to be added
    Group* newGroup = (Group*)malloc(sizeof(Group));

    // Check if malloc failed
    if (newGroup == NULL) {
        perror("Malloc Failed");
        exit(0);
    }

    // Copy the group name into the newly created group
    int size = (strlen(group_name) + 1);
    newGroup->name = (char*)malloc(size * sizeof(char));
    strncpy(newGroup->name, group_name, size);
    newGroup->name[size] = '\0';

    // Initialize the rest of the variables
    newGroup->users = NULL;
    newGroup->xcts = NULL;
    newGroup->next = NULL;

    // Assign the group to the head if it is the first in the system
    if (*group_list_ptr == NULL) {
        *group_list_ptr = newGroup;
        return 0;
    }
    else {
        // Initialize head of list
        Group* current = *group_list_ptr;

        // Find the last group in the list
        while (current != NULL) {
            // If the name already exists in the list, exit the function
            if (strcmp(current->name, group_name) == 0) {
                // Free the memory used to create the new group
                free(newGroup->name);
                free(newGroup->users);
                free(newGroup->xcts);
                free(newGroup->next);
                return -1;
            }
            else if (current->next == NULL) {
                // Assign the new group to the end of the list
                current->next = newGroup;
                return 0;
            }
            else {
                current = current->next;
            }
        }
    }
    return -1;
}

/* Print to standard output the names of all groups in group_list, one name
*  per line. Output is in the same order as group_list.
*/
void list_groups(Group *group_list) {
    // Empty system text
    if (group_list == NULL) {
        printf("There does not exists any groups\n");
    }
    else {
        // Initialize head of list
        Group* current = group_list;

        printf("List of groups:\n");

        // Print out every group in the system
        while (current != NULL) {
            printf("%s\n", current->name);
            current = current->next;
        }
    }
}

/* Search the list of groups for a group with matching group_name
* If group_name is not found, return NULL, otherwise return a pointer to the
* matching group list node.
*/
Group *find_group(Group *group_list, const char *group_name) {
  // Initialize head of list
  Group* current = group_list;

  // Find a group with the target name
  while (current != NULL) {
      // Leave function with a pointer to the group if a match was found
      if (strcmp(current->name, group_name) == 0) {
          return current;
      }
      else {
        current = current->next;
      }
  }
  // If a match was not found, return NULL
  return NULL;
}

/* Add a new user with the specified user name to the specified group. Return zero
* on success and -1 if the group already has a user with that name.
* (allocate and initialize a User data structure and insert it into the
* appropriate group list)
*/
int add_user(Group *group, const char *user_name) {
    // Create a new user to be added into a group
    User* newUser = (User*)malloc(sizeof(User));

    // Check if malloc failed
    if (newUser == NULL) {
        perror("Malloc failed");
        exit(0);
    }

    // Copy the user name into the newly created user
    int size = (strlen(user_name) + 1);
    newUser->name = (char*)malloc(size * sizeof(char));
    strncpy(newUser->name, user_name, size);
    newUser->name[size] = '\0';

    // Initialize the rest of the variables
    newUser->balance = 0.00;
    newUser->next = NULL;

    // Assign the user to the head if it is the first in the group
    if (group->users == NULL) {
        group->users = newUser;
        return 0;
    }
    else {
        // Initialize head of lists
        User* current = group->users;

        // Find the last user in the list
        while (current != NULL) {
            // If the name already exists in the list, exit the function
            if (strcmp(current->name, user_name) == 0) {
                // Free the memory used to create the new user
                free(newUser->name);
                free(newUser->next);
                return -1;
            }
            else if (current->next == NULL) {
                // Assign the new user to the end of the list
                current->next = newUser;
                return 0;
            }
            else {
                current = current->next;
            }
        }
    }
    return -1;
}

/* Remove the user with matching user and group name and
* remove all her transactions from the transaction list.
* Return 0 on success, and -1 if no matching user exists.
* Remember to free memory no longer needed.
* (Wait on implementing the removal of the user's transactions until you
* get to Part III below, when you will implement transactions.)
*/
int remove_user(Group *group, const char *user_name) {

    // Find previous user
    User* previous = find_prev_user(group, user_name);

    // If previous user is NULL, the user does not exist,
    // thus it cannot be removed
    if (previous == NULL) {
        return -1;
    }
    // Free up the memory of current and set its previous user to it's next
    else {
        // If the head of the user is being removed, there is no previous user,
        // therefore, the next user must be set as the new head
        if (strcmp(previous->name, user_name) == 0) {
            group->users = previous->next;
            free(previous->name);
            free(previous);

            // Remove all xcts related to the user
            remove_xct(group, user_name);
            return 0;
        }
        else {
            User* current = previous->next;
            previous->next = current->next;
            free(current->name);
            free(current);

            // Remove all xcts related to the user
            remove_xct(group, user_name);
            return 0;
        }
    }
}

/* Print to standard output the names of all the users in group, one
* per line, and in the order that users are stored in the list, namely
* lowest payer first.
*/
void list_users(Group *group) {
    // Initialize head of list
    User* current = group->users;

    // Empty user text
    if (current == NULL) {
        printf("There does not exists any users\n");
    }
    else {
        printf("List of users in %s\n", group->name );

        // Print out every user in the group
        while (current != NULL) {
            printf("%s\n", current->name);
            current = current->next;
        }
    }
}

/* Print to standard output the balance of the specified user. Return 0
* on success, or -1 if the user with the given name is not in the group.
*/
int user_balance(Group *group, const char *user_name) {
    // Find previous user
    User* previous = find_prev_user(group, user_name);

    // If previous is NULL, the list is either empty or the user does not exists
    if (previous == NULL) {
        return -1;
    }
    else if (strcmp(previous->name, user_name) == 0) {
        printf("User Balance: %0.2f\n", previous->balance);
        return 0;
    }
    else {
        printf("User Balance: %0.2f\n", previous->next->balance);
        return 0;
    }
}

/* Print to standard output the name of the user who has paid the least
* If there are several users with equal least amounts, all names are output.
* Returns 0 on success, and -1 if the list of users is empty.
* (This should be easy, since your list is sorted by balance).
*/
int under_paid(Group *group) {
    // Initialize head of list
    User* current = group->users;

    // Empty list text
    if (current == NULL) {
        printf("There does not exists any users\n");
        return -1;
    }
    else {
        printf("Under paid users:\n");

        // Continue to print users until the current users balance differs
        // from the following users balance
        while ((current != NULL) && (current->balance == (find_prev_user(group, current->name)->balance))) {
            printf("%s\n", current->name);
            current = current->next;
        }
        return 0;
    }
}

/* Return a pointer to the user prior to the one in group with user_name. If
* the matching user is the first in the list (i.e. there is no prior user in
* the list), return a pointer to the matching user itself. If no matching user
* exists, return NULL.
*
* The reason for returning the prior user is that returning the matching user
* itself does not allow us to change the user that occurs before the
* matching user, and some of the functions you will implement require that
* we be able to do this.
*/
User *find_prev_user(Group *group, const char *user_name) {
    // Initialize the head of the list
    User* current = group->users;

    // Return NULL if is it an empty list
    if (current == NULL) {
        return NULL;
    }
    else {
        // Continue seacrhing until the user name matches
        while (current != NULL) {
            if (strcmp(current->name, user_name) == 0) {
                return current;
            }
            current = current->next;
        }
        // If the end of the list is reached, the user does not exists
        return NULL;
    }
}

/* Add the transaction represented by user_name and amount to the appropriate
* transaction list, and update the balances of the corresponding user and group.
* Note that updating a user's balance might require the user to be moved to a
* different position in the list to keep the list in sorted order. Returns 0 on
* success, and -1 if the specified user does not exist.
*/
int add_xct(Group *group, const char *user_name, double amount) {
    // Check if the user exists in the group
    User* previous = find_prev_user(group, user_name);

    // If previous is NULL, the user did not exists in the group
    if (previous == NULL) {
        return -1;
    }
    else {
        // Create a new xct to be added into a group
        Xct* newXct = (Xct*)malloc(sizeof(Xct));

        // Copy the user name into the newly created users
        int size = (strlen(user_name) + 1);
        newXct->name = (char*)malloc(size * sizeof(char));
        strncpy(newXct->name, user_name, size);
        newXct->name[size] = '\0';

        // Initialize the rest of the variables
        newXct->amount = amount;
        newXct->next = NULL;

        // Assign the xct to the head of the list
        if (group->xcts == NULL) {
            group->xcts = newXct;
        }
        else {
            Xct* temp = group->xcts;
            group->xcts = newXct;
            newXct->next = temp;
        }

        // Add the amount towards the users balance
        if (strcmp(previous->name, user_name) == 0) {
            previous->balance = previous->balance + newXct->amount;
            // Update the user order
            sort_users(group, previous, user_name);
            return 0;
        }
        else {
            previous->next->balance = previous->next->balance + newXct->amount;
            // Update the user order
            sort_users(group, previous, user_name);
            return 0;
        }
    }
}

void sort_users(Group *group, User *previous, const char *user_name) {
    // Since only positive xct are allowed, we only need to check the users
    // following the current user, as any users before our current user will
    // already be smaller
    User* current = previous->next;

    // Temporary store the user which is being updated and close off the list as
    // if it has been removed

    // If the user is currently at the head of the list, there is no previous
    // user to link with
    if (strcmp(previous->name, user_name) == 0) {
        User* temp = previous;
        group->users = temp->next;
        // Find the proper location to insert user
        insert_user(group, temp->next, temp);
    }
    else {
        User* temp = current;
        previous->next = current->next;
        // Find the proper location to insert user
        insert_user(group, temp->next, temp);
    }
}

void insert_user(Group *group, User* current, User* user) {
    // Find the first user which holds a greater balance then the current user
    // and set it as the current users next pointer
    while (current != NULL) {
        // Once a user with a greater balance is found, set our current user as
        // it's previous
        if (current->balance > user->balance) {
            User* greaterPrev = find_prev_user(group, current->name);
            greaterPrev->next = user;
            user->next = current;
            break;
        }
        else {
            // If the next user is NULL, the end of the list is reached
            // therefore the user will be set as the tail
            if (current->next == NULL) {
                current->next = user;
                user->next = NULL;
                break;
            }
            else {
                current = current->next;
            }
        }
    }
}
/* Print to standard output the num_xct most recent transactions for the
* specified group (or fewer transactions if there are less than num_xct
* transactions posted for this group). The output should have one line per
* transaction that prints the name and the amount of the transaction. If
* there are no transactions, this function will print nothing.
*/
void recent_xct(Group *group, long nu_xct) {
    // Initialize head of list
    Xct* current = group->xcts;

    int count = (int)nu_xct;

    // Empty list text
    if (current == NULL) {
        printf("There does not exists any transactions\n");
    }
    else {
        printf("Most recent transactions:\n");

        // Print out the first 'count' xcts in the list
        int i;
        for (i = 0; i < count; i++) {
            // If current is NULL, the end of the list is reached and the loop
            // will stop
            if (current != NULL) {
                printf("%s: %0.2f\n", current->name, current->amount);
                current = current->next;
            }
            else {
              break;
            }
        }
    }
}

/* Remove all transactions that belong to the user_name from the group's
* transaction list. This helper function should be called by remove_user.
* If there are no transactions for this user, the function should do nothing.
* Remember to free memory no longer needed.
*/
void remove_xct(Group *group, const char *user_name) {
    // Initialize head of list
    Xct* current = group->xcts;

    // Initialize a pointer to the previous user
    Xct* previous = NULL;

    // Check every xct in the group, if any of them hold the user_name,
    // remove it from the list
    while (current != NULL) {
        if (strcmp(current->name, user_name) == 0) {
            Xct* temp = current;
            // If the head is being removed, assign its next xct as the new head
            if (previous == NULL) {
                group->xcts = current->next;
            }
            else {
                previous->next = current->next;
            }

            // Free up memeory used create xct
            free(temp->name);
            free(temp->next);
            free(temp);
        }
        previous = current;
        current = current->next;
    }
}
