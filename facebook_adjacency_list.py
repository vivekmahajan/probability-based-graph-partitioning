#This script to convert the facebook graph into formats we want. First one is for metis and the second one is for edge weights
#usage python facebook_adjacency_list.py >graph_with_weights 2>graph_metis
import sys

user_access = {}

for wall_post in open("./datasets/facebook-wall.txt.anon", "r"):
    users = wall_post.split("\t")
    if user_access.has_key((int(users[0]), int(users[1]))):
        user_access[(int(users[0]), int(users[1]))] += 1
    elif user_access.has_key((int(users[1]), int(users[0]))):
        user_access[(int(users[1]), int(users[0]))] += 1
    else:
        user_access[(int(users[0]), int(users[1]))] = 1


for key in user_access.keys():
    user_access[key] = user_access[key]/2
    #print key," ", user_access[key]

adjacency_list = {}
for friends in open("./datasets/facebook-links.txt.anon", "r"):
    users = friends.split("\t")
    user1 = int(users[0])
    user2 = int(users[1])
    if not adjacency_list.has_key(user1):
        adjacency_list[user1] = {}
    if not adjacency_list.has_key(user2):
        adjacency_list[user2] = {}
     
    if user_access.has_key((user1, user2)):
        adjacency_list[user1][str(user2)] = user_access[(user1, user2)] 
        adjacency_list[user2][str(user1)] = user_access[(user1, user2)]   
    elif user_access.has_key((user2, user1)):
        adjacency_list[user1][str(user2)] = user_access[(user2, user1)] 
        adjacency_list[user2][str(user1)] = user_access[(user2, user1)]   
    else:
        adjacency_list[user1][str(user2)] = 0 
        adjacency_list[user2][str(user1)] = 0   

keys = []
for key in (adjacency_list.keys()):
    keys.append(key)
    #print "**",key,"**", " ".join(adjacency_list[key])
keys.sort()
edges = 0

for key in keys :
    edges = edges + len(adjacency_list[key])

print len(adjacency_list), edges/2

for key in keys :
    for edge in adjacency_list[key].keys():
        sys.stdout.write(str(edge)+","+str(adjacency_list[key][edge])+" ")
    sys.stdout.write('\n')

print >> sys.stderr, len(adjacency_list),edges/2

for key in keys :
    print >> sys.stderr, " ".join(adjacency_list[key].keys())
