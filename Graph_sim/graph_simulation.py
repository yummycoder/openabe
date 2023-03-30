import networkx as nx
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("init", type=argparse.FileType('rb'))
parser.add_argument("-a", dest="addFile", type=argparse.FileType('r'))
parser.add_argument("-r", dest="removeFile", type=argparse.FileType('r'))
args = parser.parse_args()

init_file = args.init
G = nx.read_edgelist(init_file, create_using=nx.DiGraph)
init_file.close()

ancesstor_dict = dict()
for n in G:
    ancesstor_dict[n] = nx.ancestors(G, n)
if args.addFile is not None:
    # Add relation ship
    add_list = []
    lines = args.addFile.readlines()
    for line in lines:
        add_list.append(tuple(line.split()))
    G.add_edges_from(add_list)

    for n in G:
        if n in ancesstor_dict:
            diff = nx.ancestors(G, n) - ancesstor_dict[n]
        if len(diff) > 0:
            print(n + '\t' + str(diff) + '\t' + str(len(diff)))

elif args.removeFile is not None:
    remove_list = []
    # remove_file = open("./remove_node.txt", "r")
    lines = args.removeFile.readlines()
    for line in lines:
        remove_list.append(line)
    G.remove_nodes_from(remove_list)

    for n in G:
        if n in ancesstor_dict:
            diff = ancesstor_dict[n] - nx.ancestors(G, n)
        if len(diff) > 0:
            print(n + '\t' + str(diff) + '\t' + str(len(diff)))
