
# stdlib
import os
import matplotlib
import numpy as np      # Configure seed
import math
import matplotlib.pyplot as plt
import numexpr as ne
import imageio
import pylab

# ourlib
from pytools import Reader      # Parse training/testing files

outdir = "out"
temp_filename = ""
replace_str = ".Master.log"
x_range = []

# Global Setting Values
i_time = 0
i_gen = 1
i_type = 2
i_agent = 3

'''
def OutGenerationSummary(data, filename):

    # Structure of the csv at this point
    i_model = 3
    i_fitness = 4
    i_error = 5
    data = data[ data[:,1] == 'PopulationBestFitness' ]
    

    # Filename for this type of output
    main_filename = filename.replace(replace_str,".Master.GenerationSummary.png")
    evolve_x_filename = filename.replace(replace_str,".Master.Evolve.X.gif")
    evolve_filename = filename.replace(replace_str,".Master.Evolve.gif")

    # Get data from output
    X = []      # Generation X
    Y = []      # Fitness Y
    
    # Get train and test data and prepare to evaluate the interim models
    PredTrainYs = []     # Predicted Y
    PredTestYs = []     # Predicted Y
    XsTrain, YTrain, feats, dYTrain, WTrain, _ = Reader.CsvDatasetFile( filename.replace(replace_str, ".Train.csv"))
    XsTest, YTest, _, dYTest, WTest, _ = Reader.CsvDatasetFile( filename.replace(replace_str, ".Test.csv") )
    XsAll = np.concatenate( (XsTrain,XsTest), axis=0 )
    YAll = np.concatenate( (YTrain,YTest), axis=0 )
    train_eval = {}
    test_eval = {}
    all_eval = {}
    for j in range(0, len(feats)):
        train_eval['x'+str(j+1)] = XsTrain[:,j]
        test_eval['x'+str(j+1)] = XsTest[:,j]
        all_eval['x'+str(j+1)] = XsAll[:,j]

    lim_min = np.min(YAll)-(np.max(YAll)-np.min(YAll))*.1
    lim_max = np.max(YAll)+(np.max(YAll)-np.min(YAll))*.1

    evolve_images = []
    i = 0
    best_fitness = -1
    for d in data:

        # Append generational fitness
        X.append(i)
        Y.append(float(d[i_fitness]))

        # Create predictions
        model = str(d[i_model])
        PredTestYs.append(ne.evaluate(model, test_eval))

        if i == 0:
            
            best_fitness = float(d[i_fitness])

            # Training evolution Relative
            plt.figure(np.random.randint(0,99999999), figsize=(45,15))
            fig, axs = plt.subplots(1,3,figsize=(45,15),sharey=True,sharex=True)
            #fig.suptitle(f"Evolution At Generation {i} Relative Plot \n\n Fitness: {float(d[i_fitness])} Error: {float(d[i_error])} \n\n",fontsize=25)
            fig.suptitle(f"End of Evolution \n\n",fontsize=25)
            fig.tight_layout(pad=0.8)
            axs[0].axis('off')
            axs[1].axis('off')
            axs[2].axis('off')
            plt.savefig(f"{temp_dir}/Empty.png")
            plt.close()

        if best_fitness > float(d[i_fitness]) or i == 0:

            best_fitness = float(d[i_fitness])
            
            # Training evolution Relative
            plt.figure(np.random.randint(0,99999999), figsize=(45,15))
            fig, axs = plt.subplots(1,3,figsize=(45,15),sharey=True,sharex=True)
            
            plt.setp(axs, xlim=(lim_min,lim_max), ylim=(lim_min,lim_max))

            fig.suptitle(f"Relative Evolution At Generation {i} ",fontsize=25)
            fig.tight_layout(pad=3.0)
            fig.subplots_adjust(top=0.8)
            
            axs[0].set_title(f"Training Performance \n\n Fitness: {float(d[i_fitness])} Error: {float(d[i_error])} \n",size=20)
            axs[0].plot(YTrain,YTrain, c='blue', label='Reference')
            axs[0].scatter(YTrain, ne.evaluate(model, train_eval), c='black', label='Actual Vs Predicted')
            
            axs[1].set_title("\n Testing Performance \n\n\n\n",size=20)
            axs[1].plot(YTest,YTest, c='blue', label='Reference')
            axs[1].scatter(YTest, ne.evaluate(model, test_eval), c='black', label='Actual Vs Predicted')
            
            axs[2].set_title("\n Full Performance \n\n\n\n",size=20)
            axs[2].plot(YAll,YAll, c='blue', label='Reference')
            axs[2].scatter(YAll, ne.evaluate(model, all_eval), c='black', label='Actual Vs Predicted')

            plt.savefig(f"{temp_dir}/{i}.Train.png")
            plt.close()
            evolve_images.append(imageio.imread(f"{temp_dir}/{i}.Train.png"))

            # Testing Evolution
            #plt.figure(np.random.randint(0,99999999))
            #plt.set_title("Training Evolution",size=20)
            #plt.plot(XsTest, YTest, c='blue', label='Ground Test')
            #plt.plot(XsTrain, ne.evaluate(model, train_eval), c='blue', label='Ground Train')

        i = i+1                
    
    evolve_images.append( imageio.imread(f"{temp_dir}/Empty.png"))
    imageio.mimsave(evolve_filename, evolve_images, fps=0.25)

    plt.figure(np.random.randint(0,99999999))
    fig, axs = plt.subplots(1,1,figsize=(25,25),sharey=True,sharex=True)
    fig.suptitle("Generational Summary Plot",fontsize=25)
    fig.tight_layout(pad=3.0)

    #axs[0, 0].set_title("Root Agent Performance",size=20)
    axs.set_title("Root Agent Performance",size=20)
    axs.plot(X, Y, c='green', label='Pocket Fitness')

    plt.savefig(main_filename)
'''

def GetPositiveMutations(data, upto, X, PocketYs):

    # Prepare data
    Xs = []
    Ys = []
    for i in range(0,13):
        Xs.append([])
        Ys.append([])

    # Structure of the csv at this point
    i_pre_model = i_agent+1
    i_pre_fitness = i_agent+2
    i_pre_error = i_agent+3
    i_post_model = i_agent+4
    i_post_fitness = i_agent+5
    i_post_error = i_agent+6
    
    data = data[ data[:,i_type] == 'AgentCurrentMutate' ]

    offset = 0
    last_gen = 0
    for d in data:

        print(f"Mutate For gen:{d[i_gen]} agent:{d[i_agent]} log:{d[i_type]} pre:{d[i_pre_fitness]} post:{d[i_post_fitness]}")

        if int(d[i_gen]) >= upto:
            return Xs, Ys

        if not math.isnan(float(d[i_post_fitness])) and float(d[i_post_fitness]) < float(d[i_pre_fitness]):

            print(f"Tracking Mutation For gen:{d[i_gen]} agent:{d[i_agent]} log:{d[i_type]} pre:{d[i_pre_fitness]} post:{d[i_post_fitness]}")

            # Flag generation as positve mutation
            Xs[ d[i_agent] ].append( d[i_gen] )

            # Find the fitness at this generation
            for idx, val in enumerate(X):
                if val == (int(d[i_gen])):
                    Ys[ d[i_agent] ].append( PocketYs[d[i_agent]][idx] )
                    break
    
    return Xs, Ys

def GetBubbleUp(data, upto, X, PocketYs):   

    # Prepare data
    Xs = []
    Ys = []
    Cs = []
    for i in range(0,13):
        Xs.append([])
        Ys.append([])
        Cs.append([])

    # Structure of the csv at this point
    i_pre_model = i_agent+1
    i_pre_fitness = i_agent+2
    i_pre_error = i_agent+3
    i_child = i_agent+4
    i_child_model = i_agent+5
    i_child_fitness = i_agent+6
    i_child_error = i_agent+7
    
    data = data[ data[:,i_type] == 'AgentBubbleUp' ]
    for d in data:

        print(f"Bubble For gen:{d[i_gen]} agent:{d[i_agent]} log:{d[i_type]} pre:{d[i_pre_fitness]} post:{d[i_child_fitness]}")

        if int(d[i_gen]) >= upto:
            return Xs, Ys, Cs

        # Flag generation as positve mutation
        Xs[ d[i_agent] ].append( d[i_gen] )

        # Determine degree of child
        Cs[ d[i_agent] ].append( int(d[i_child])%3 )

        # Find the fitness at this generation
        for idx, val in enumerate(X):
            if val == (int(d[i_gen])):
                Ys[ d[i_agent] ].append( PocketYs[d[i_agent]][idx] )
                break
    
    return Xs, Ys, Cs

def GetPocketLocalSearch(data, upto, X, PocketYs):   

    # Prepare data
    Xs = []
    Ys = []
    for i in range(0,13):
        Xs.append([])
        Ys.append([])

    # Structure of the csv at this point
    i_pre_model = i_agent+1
    i_pre_fitness = i_agent+2
    i_pre_error = i_agent+3
    i_post_model = i_agent+4
    i_post_fitness = i_agent+5
    i_post_error = i_agent+6
    
    data = data[ data[:,i_type] == 'AgentPocketLocalSearch' ]
    for d in data:

        print(f"Pocket LS For gen:{d[i_gen]} agent:{d[i_agent]} log:{d[i_type]} pre:{d[i_pre_fitness]} post:{d[i_post_fitness]}")

        if int(d[i_gen]) >= upto:
            return Xs, Ys

        if float(d[i_post_fitness]) < float(d[i_pre_fitness]):

            print(f"Actioning Pocket LS For gen:{d[i_gen]} agent:{d[i_agent]} log:{d[i_type]} pre:{d[i_pre_fitness]} post:{d[i_post_fitness]}")

            if int(d[i_gen]) not in Xs[ int(d[i_agent]) ]:

                Xs[ d[i_agent] ].append( int(d[i_gen]) )

                # Find the fitness at this generation
                for idx, val in enumerate(X):
                    if val == (int(d[i_gen])):
                        Ys[ d[i_agent] ].append( PocketYs[d[i_agent]][idx] )
                        break
    
    return Xs, Ys

def GetSuccessfulRecombinations(data, upto, PocketYs):

    # Prepare data
    Xs = []
    Ys = []
    for i in range(0,13):
        Xs.append([])
        Ys.append([])

    # Structure of the csv at this point
    i_pre_model = i_agent+1
    i_pre_fitness = i_agent+2
    i_pre_error = i_agent+3
    i_post_model = i_agent+4
    i_post_model = i_agent+5
    i_post_model = i_agent+6
    
    temp_data = data
    data = []
    for d in temp_data:
        if d[i_type] == 'AgentConstruct' or d[i_type] == 'AgentCurrentRecombine' or d[i_type] == 'PopulationBestFitnessPocket':
            data.append(d)
    
    for d in data:

        if d[i_type] == 'AgentCurrentMutate' or d[i_type] == 'AgentConstruct':

            if d[i_gen] == upto:
                return Xs, Ys

            Xs[ d[i_agent] ].append(d[i_gen])
            Ys[ d[i_agent] ].append(PocketYs[d[i_agent]][d[i_gen]])
        
    return Xs, Ys

def GetColor(children):

    ret = []
    for child in children:

        if child == 0: ret.append('blue')
        if child == 1: ret.append('red' )
        if child == 2: ret.append('grey')

    return ret

def GetCounts(agents):
    
    ret = []
    for i in range(0,13):
        ret.append([0, 0, 0])

    for agent in range(0,len(agents)):
        for child in agents[agent]:
            if child == 0: ret[agent][0] = ret[agent][0]+1
            if child == 1: ret[agent][1] = ret[agent][1]+1
            if child == 2: ret[agent][2] = ret[agent][2]+1

    return ret

def Subplot(axs, row, col, agent_no, X, Ys, mutate_xs, mutate_ys, bubble_up_xs, bubble_up_ys, bubble_up_cs, pock_ls_xs, pock_ls_ys):

    s_mutate = ''
    s_local = ''
    s_fitness = ''
    s_bubble = ''
    if row == 0:
        s_mutate = 'Mutate Improve'
        s_local = 'LS Improve'
        s_fitness = 'Pocket Fitness'
        s_bubble = 'Bubble Improve From Child\nBlue = left, Red = Center, Gray = Right\n'


    axs[row, col].set_title( f"Agent {agent_no}", size=20)

    axs[row, col].plot(X, Ys[agent_no], c='green', label=s_fitness)
    
    if mutate_xs is not None and mutate_ys is not None:
        axs[row, col].scatter(mutate_xs[agent_no], mutate_ys[agent_no], marker='D', c='black', label=f"({len(mutate_xs[agent_no])}) "+s_mutate)
    
    if bubble_up_xs is not None and bubble_up_ys is not None and row < 2:
        counts = GetCounts(bubble_up_cs)
        axs[row, col].scatter(bubble_up_xs[agent_no], bubble_up_ys[agent_no], marker='^', c=GetColor(bubble_up_cs[agent_no]), label=s_bubble+f"({counts[agent_no][0]}) ({counts[agent_no][1]}) ({counts[agent_no][2]})")
    
    if pock_ls_xs is not None and pock_ls_ys is not None:
        axs[row, col].scatter(pock_ls_xs[agent_no], pock_ls_ys[agent_no], marker='o', c='black', label=f"({len(pock_ls_xs[0])}) "+s_local)
    
    # Legend
    if row < 2:     axs[row, col].legend(bbox_to_anchor=(1.04,0.5), loc="center left")
    else:           axs[row, col].legend(bbox_to_anchor=(1.04,0.5), loc="lower center")

    # Force ints on xaxis
    axs[row, col].xaxis.get_major_locator().set_params(integer=True)

    # Turn on (all turned off)
    axs[row, col].axis('on')


def GenerationalSummaryGrid(file, X, Ys, mutate_xs = None, mutate_ys = None, bubble_up_xs = None, bubble_up_ys = None, bubble_up_cs = None,
    pock_ls_xs = None, pock_ls_ys = None):

    # Setup the plot
    plt.figure(np.random.randint(0,99999999))
    fig, axs = plt.subplots(3,9,figsize=(50,20),sharex=True)
    fig.suptitle("Generational Summary Plot",fontsize=25)
    fig.tight_layout(pad=3.0)

    # Turn off all axes
    for ax in axs.reshape(-1):
        ax.axis('off')

    # Display Agents
    Subplot(axs,0,4,0,X,Ys,mutate_xs, mutate_ys, bubble_up_xs, bubble_up_ys, bubble_up_cs, pock_ls_xs, pock_ls_ys)
    Subplot(axs,1,1,1,X,Ys,mutate_xs, mutate_ys, bubble_up_xs, bubble_up_ys, bubble_up_cs, pock_ls_xs, pock_ls_ys)
    Subplot(axs,1,4,2,X,Ys,mutate_xs, mutate_ys, bubble_up_xs, bubble_up_ys, bubble_up_cs, pock_ls_xs, pock_ls_ys)
    Subplot(axs,1,7,3,X,Ys,mutate_xs, mutate_ys, bubble_up_xs, bubble_up_ys, bubble_up_cs, pock_ls_xs, pock_ls_ys)
    Subplot(axs,2,0,4,X,Ys,mutate_xs, mutate_ys, bubble_up_xs, bubble_up_ys, bubble_up_cs, pock_ls_xs, pock_ls_ys)
    Subplot(axs,2,1,5,X,Ys,mutate_xs, mutate_ys, bubble_up_xs, bubble_up_ys, bubble_up_cs, pock_ls_xs, pock_ls_ys)
    Subplot(axs,2,2,6,X,Ys,mutate_xs, mutate_ys, bubble_up_xs, bubble_up_ys, bubble_up_cs, pock_ls_xs, pock_ls_ys)
    Subplot(axs,2,3,7,X,Ys,mutate_xs, mutate_ys, bubble_up_xs, bubble_up_ys, bubble_up_cs, pock_ls_xs, pock_ls_ys)
    Subplot(axs,2,4,8,X,Ys,mutate_xs, mutate_ys, bubble_up_xs, bubble_up_ys, bubble_up_cs, pock_ls_xs, pock_ls_ys)
    Subplot(axs,2,5,9,X,Ys,mutate_xs, mutate_ys, bubble_up_xs, bubble_up_ys, bubble_up_cs, pock_ls_xs, pock_ls_ys)
    Subplot(axs,2,6,10,X,Ys,mutate_xs, mutate_ys, bubble_up_xs, bubble_up_ys, bubble_up_cs, pock_ls_xs, pock_ls_ys)
    Subplot(axs,2,7,11,X,Ys,mutate_xs, mutate_ys, bubble_up_xs, bubble_up_ys, bubble_up_cs, pock_ls_xs, pock_ls_ys)
    Subplot(axs,2,8,12,X,Ys,mutate_xs, mutate_ys, bubble_up_xs, bubble_up_ys, bubble_up_cs, pock_ls_xs, pock_ls_ys)

    plt.gca().xaxis.get_major_locator().set_params(integer=True)

    plt.savefig(file)
    plt.close()


    '''
    axs[0, 4].set_title("Agent 0",size=20)
    axs[0, 4].plot(X, Ys[0], c='green', label='Pocket Fitness')
    if mutate_xs is not None and mutate_ys is not None:
        axs[0, 4].scatter(mutate_xs[0], mutate_ys[0], marker='D', c='black', label=f"({len(mutate_xs[0])}) "+s_mutate)
    if bubble_up_xs is not None and bubble_up_ys is not None:
        counts = GetCounts(bubble_up_cs)
        axs[0, 4].scatter(bubble_up_xs[0], bubble_up_ys[0], marker='^', c=GetColor(bubble_up_cs[0]), label=s_bubble+f"\n({counts[0][0]}) ({counts[0][1]}) ({counts[0][2]})")
    if pock_ls_xs is not None and pock_ls_ys is not None:
        axs[0, 4].scatter(pock_ls_xs[0], pock_ls_ys[0], marker='o', c='black', label=f"({len(pock_ls_xs[0])}) "+s_local)
    axs[0, 4].legend(bbox_to_anchor=(1.04,0.5), loc="center left")
    axs[0, 4].xaxis.get_major_locator().set_params(integer=True)
    axs[0, 0].axis('off')
    axs[0, 1].axis('off')
    axs[0, 2].axis('off')
    axs[0, 3].axis('off')
    axs[0, 5].axis('off')
    axs[0, 6].axis('off')
    axs[0, 7].axis('off')
    axs[0, 8].axis('off')

    # Depth 1
    axs[1, 1].set_title("Agent 1",size=20)
    axs[1, 1].plot(X, Ys[1], c='green', label='Pocket Fitness')
    if mutate_xs is not None and mutate_ys is not None:
        axs[1, 1].scatter(mutate_xs[1], mutate_ys[1], marker='D', c='black', label=f"({len(mutate_xs[1])}) "+s_mutate)
    if bubble_up_xs is not None and bubble_up_ys is not None:
        axs[1, 1].scatter(bubble_up_xs[1], bubble_up_ys[1], marker='^', c=GetColor(bubble_up_cs[1]), label='Mutation')
    if pock_ls_xs is not None and pock_ls_ys is not None:
        axs[1, 1].scatter(pock_ls_xs[1], pock_ls_ys[1], marker='o', c='black', label='Pocket LS')
    axs[1, 1].legend(bbox_to_anchor=(1.04,0.5), loc="center left")
    
    axs[1, 4].set_title("Agent 2",size=20)
    axs[1, 4].plot(X, Ys[2], c='green', label='Pocket Fitness')
    if mutate_xs is not None and mutate_ys is not None:
        axs[1, 4].scatter(mutate_xs[2], mutate_ys[2], marker='D', c='black', label='Mutation')
        axs[1, 1].scatter(mutate_xs[1], mutate_ys[1], marker='D', c='black', label=f"({len(mutate_xs[1])}) "+s_mutate)
    if bubble_up_xs is not None and bubble_up_ys is not None:
        axs[1, 4].scatter(bubble_up_xs[2], bubble_up_ys[2], marker='^', c=GetColor(bubble_up_cs[2]), label='Bubbled Up')
    if pock_ls_xs is not None and pock_ls_ys is not None:
        axs[1, 4].scatter(pock_ls_xs[2], pock_ls_ys[2], marker='o', c='black', label='Pocket LS')
    axs[1, 7].set_title("Agent 3",size=20)
    axs[1, 7].plot(X, Ys[3], c='green', label='Pocket Fitness')
    if mutate_xs is not None and mutate_ys is not None:
        axs[1, 7].scatter(mutate_xs[3], mutate_ys[3], marker='D', c='black', label='Mutation')
    if bubble_up_xs is not None and bubble_up_ys is not None:
        axs[1, 7].scatter(bubble_up_xs[3], bubble_up_ys[3], marker='^', c=GetColor(bubble_up_cs[3]), label='Bubbled Up')
    if pock_ls_xs is not None and pock_ls_ys is not None:
        axs[1, 7].scatter(pock_ls_xs[3], pock_ls_ys[3], marker='o', c='black', label='Pocket LS')
    axs[1, 0].axis('off')
    axs[1, 2].axis('off')
    axs[1, 3].axis('off')
    axs[1, 5].axis('off')
    axs[1, 6].axis('off')
    axs[1, 8].axis('off')

    # Depth 2
    axs[2, 0].set_title("Agent 4",size=20)
    axs[2, 0].plot(X, Ys[4], c='green', label='Pocket Fitness')
    if mutate_xs is not None and mutate_ys is not None:
        axs[2, 0].scatter(mutate_xs[4], mutate_ys[4], marker='D', c='black', label='Mutation')
    if pock_ls_xs is not None and pock_ls_ys is not None:
        axs[2, 0].scatter(pock_ls_xs[4], pock_ls_ys[4], marker='o', c='black', label='Pocket LS')
    axs[2, 1].set_title("Agent 5",size=20)
    axs[2, 1].plot(X, Ys[5], c='green', label='Pocket Fitness')
    if mutate_xs is not None and mutate_ys is not None:
        axs[2, 1].scatter(mutate_xs[5], mutate_ys[5], marker='D', c='black', label='Mutation')
    if pock_ls_xs is not None and pock_ls_ys is not None:
        axs[2, 1].scatter(pock_ls_xs[5], pock_ls_ys[5], marker='o', c='black', label='Pocket LS')
    axs[2, 2].set_title("Agent 6",size=20)
    axs[2, 2].plot(X, Ys[6], c='green', label='Pocket Fitness')
    if mutate_xs is not None and mutate_ys is not None:
        axs[2, 2].scatter(mutate_xs[6], mutate_ys[6], marker='D', c='black', label='Mutation')
    if pock_ls_xs is not None and pock_ls_ys is not None:
        axs[2, 2].scatter(pock_ls_xs[6], pock_ls_ys[6], marker='o', c='black', label='Pocket LS')

    axs[2, 3].set_title("Agent 7",size=20)
    axs[2, 3].plot(X, Ys[7], c='green', label='Pocket Fitness')
    if mutate_xs is not None and mutate_ys is not None:
        axs[2, 3].scatter(mutate_xs[7], mutate_ys[7], marker='D', c='black', label='Mutation')
    if pock_ls_xs is not None and pock_ls_ys is not None:
        axs[2, 3].scatter(pock_ls_xs[7], pock_ls_ys[7], marker='o', c='black', label='Pocket LS')
    axs[2, 4].set_title("Agent 8",size=20)
    axs[2, 4].plot(X, Ys[8], c='green', label='Pocket Fitness')
    if mutate_xs is not None and mutate_ys is not None:
        axs[2, 4].scatter(mutate_xs[8], mutate_ys[8], marker='D', c='black', label='Mutation')
    if pock_ls_xs is not None and pock_ls_ys is not None:
        axs[2, 4].scatter(pock_ls_xs[8], pock_ls_ys[8], marker='o', c='black', label='Pocket LS')
    axs[2, 5].set_title("Agent 9",size=20)
    axs[2, 5].plot(X, Ys[9], c='green', label='Pocket Fitness')
    if mutate_xs is not None and mutate_ys is not None:
        axs[2, 5].scatter(mutate_xs[9], mutate_ys[9], marker='D', c='black', label='Mutation')
    if pock_ls_xs is not None and pock_ls_ys is not None:
        axs[2, 5].scatter(pock_ls_xs[9], pock_ls_ys[9], marker='o', c='black', label='Pocket LS')

    axs[2, 6].set_title("Agent 10",size=20)
    axs[2, 6].plot(X, Ys[10], c='green', label='Pocket Fitness')
    if mutate_xs is not None and mutate_ys is not None:
        axs[2, 6].scatter(mutate_xs[10], mutate_ys[10], marker='D', c='black', label='Mutation')
    if pock_ls_xs is not None and pock_ls_ys is not None:
        axs[2, 6].scatter(pock_ls_xs[10], pock_ls_ys[10], marker='o', c='black', label='Pocket LS')
    axs[2, 7].set_title("Agent 11",size=20)
    axs[2, 7].plot(X, Ys[11], c='green', label='Pocket Fitness')
    if mutate_xs is not None and mutate_ys is not None:
        axs[2, 7].scatter(mutate_xs[11], mutate_ys[11], marker='D', c='black', label='Mutation')
    if pock_ls_xs is not None and pock_ls_ys is not None:
        axs[2, 7].scatter(pock_ls_xs[11], pock_ls_ys[11], marker='o', c='black', label='Pocket LS')
    axs[2, 8].set_title("Agent 12",size=20)
    axs[2, 8].plot(X, Ys[12], c='green', label='Pocket Fitness')
    if mutate_xs is not None and mutate_ys is not None:
        axs[2, 8].scatter(mutate_xs[12], mutate_ys[12], marker='D', c='black', label='Mutation')
    if pock_ls_xs is not None and pock_ls_ys is not None:
        axs[2, 8].scatter(pock_ls_xs[12], pock_ls_ys[12], marker='o', c='black', label='Pocket LS')
    '''

def GenerationalSummary(orig_data, filename):

    # !!! WORKGING HERE, need to make the Agent grid showing Pocket Values
    # !!! Then

    finished_file = filename.replace(replace_str,".Master.GenerationSummary.gif")

    # Structure of the csv at this point
    i_fitness = i_agent+2
    data = orig_data[ orig_data[:,i_type] == 'PopulationBestFitnessPocket' ]
    
    # Create Space for 13 Agents
    X = []
    Ys = []
    for i in range(0,13):
        Ys.append([])
    images = []
    XsMutate = []
    YsMutate = []

    # For each data sample of PopulationBestFitnessPocket - i.e. only one per agent per generation 
    gen_filename = ""
    last_gen = 0
    X.append(last_gen)
    for d in data:

        if last_gen != int(d[i_gen]):

            # Print every fith
            if int(d[i_gen]+1) % 20 == 0 or int(d[i_gen]) == 1:

                XsMutate, YsMutate = GetPositiveMutations(orig_data, int(d[i_gen]), X, Ys)
                XsBubbleUp, YsBubbleUp, CsBubbleUp = GetBubbleUp(orig_data, int(d[i_gen]), X, Ys)
                XsPockLS, YsPockLS = GetPocketLocalSearch(orig_data, int(d[i_gen]), X, Ys)
            
                gen_filename = temp_filename.replace(replace_str, f".Master.GenerationSummary.{d[i_gen]}.png")
                GenerationalSummaryGrid(gen_filename, X, Ys, mutate_xs = XsMutate, mutate_ys = YsMutate, bubble_up_xs = XsBubbleUp, bubble_up_ys = YsBubbleUp, bubble_up_cs = CsBubbleUp,
                                    pock_ls_xs = XsPockLS, pock_ls_ys = YsPockLS)

                images.append(imageio.imread(gen_filename))

            X.append(d[i_gen])
            last_gen = int(d[i_gen])
        
        Ys[d[i_agent]].append(float(d[i_fitness]))

    # Do at last generation
    gen_filename = temp_filename.replace(replace_str, f".Master.GenerationSummary.{ data[-1][i_gen]}.png")
    GenerationalSummaryGrid(gen_filename, X, Ys,mutate_xs = XsMutate, mutate_ys = YsMutate, bubble_up_xs = XsBubbleUp, bubble_up_ys = YsBubbleUp, bubble_up_cs = CsBubbleUp)
    images.append(imageio.imread(gen_filename))
    images.append(imageio.imread(gen_filename))
    images.append(imageio.imread(gen_filename))
    
    # Build CSV
    imageio.mimsave(finished_file, images, fps=1)

def OutLocalSearch(data, filename):

    data = data[ data[:,1] == 'AgentPocketLocalSearch' ]

    # Create Space for 13 Agents
    Xs = []
    Ys = []
    for i in range(0,13):
        Xs.append([])
        Ys.append([])

    # For all the pocket local search data
    for d in data:

        did_improve = float(d[4]) > float(d[7])
        Xs[d[2]].append(float(d[0]))                                # Append x-axis value to the respective agent

        if did_improve:         Ys[d[2]].append(float(d[7]))        # Append new value to the respective agent
        else:                   Ys[d[2]].append(-1)                 # Append no improvement to respective agent
        
    # Setup the plot
    plt.figure(np.random.randint(0,99999999))
    fig, axs = plt.subplots(3,9,figsize=(50,20),sharey=True,sharex=True)
    fig.suptitle("Local Search Plot",fontsize=25)
    fig.tight_layout(pad=3.0)
    
    # Depth 0
    axs[0, 4].set_title("Agent 0",size=20)
    axs[0, 4].plot(Xs[0], Ys[0], c='green', label='Pocket Fitness')
    #axs[0, 4].axvspan(Xs[0], 0, 100, where=Ys[0] == -1)

    axs[0, 0].axis('off')
    axs[0, 1].axis('off')
    axs[0, 2].axis('off')
    axs[0, 3].axis('off')
    axs[0, 5].axis('off')
    axs[0, 6].axis('off')
    axs[0, 7].axis('off')
    axs[0, 8].axis('off')

    # Depth 1
    axs[1, 1].set_title("Agent 1",size=20)
    axs[1, 1].plot(Xs[1], Ys[1], c='green', label='Pocket Fitness')
    
    axs[1, 4].set_title("Agent 2",size=20)
    axs[1, 4].plot(Xs[2], Ys[2], c='green', label='Pocket Fitness')
    axs[1, 7].set_title("Agent 3",size=20)
    axs[1, 7].plot(Xs[3], Ys[3], c='green', label='Pocket Fitness')
    axs[1, 0].axis('off')
    axs[1, 2].axis('off')
    axs[1, 3].axis('off')
    axs[1, 5].axis('off')
    axs[1, 6].axis('off')
    axs[1, 8].axis('off')


    # Depth 2
    axs[2, 0].set_title("Agent 4",size=20)
    axs[2, 0].plot(Xs[4], Ys[4], c='green', label='Pocket Fitness')
    axs[2, 1].set_title("Agent 5",size=20)
    axs[2, 1].plot(Xs[5], Ys[5], c='green', label='Pocket Fitness')
    axs[2, 2].set_title("Agent 6",size=20)
    axs[2, 2].plot(Xs[6], Ys[6], c='green', label='Pocket Fitness')

    axs[2, 3].set_title("Agent 7",size=20)
    axs[2, 3].plot(Xs[7], Ys[7], c='green', label='Pocket Fitness')
    axs[2, 4].set_title("Agent 8",size=20)
    axs[2, 4].plot(Xs[8], Ys[8], c='green', label='Pocket Fitness')
    axs[2, 5].set_title("Agent 9",size=20)
    axs[2, 5].plot(Xs[9], Ys[9], c='green', label='Pocket Fitness')

    axs[2, 6].set_title("Agent 10",size=20)
    axs[2, 6].plot(Xs[10], Ys[10], c='green', label='Pocket Fitness')
    axs[2, 7].set_title("Agent 11",size=20)
    axs[2, 7].plot(Xs[11], Ys[11], c='green', label='Pocket Fitness')
    axs[2, 8].set_title("Agent 12",size=20)
    axs[2, 8].plot(Xs[12], Ys[12], c='green', label='Pocket Fitness')

    #handles, labels = axs[row,col].get_legend_handles_labels()
    #plt.legend(handles, labels, loc='upper center')

    plt.savefig(filename.replace(replace_str,".Master.PocketFitness.png"))
    plt.close()

for filename in os.listdir(outdir):

    if filename.endswith(".Master.log"):

        temp_filename = f"{outdir}/temp/{filename}"
        cwd = os.getcwd()
        filename = os.path.join(cwd, f"{outdir}/{filename}")
        temp_filename = f"{cwd}/{temp_filename}"

        if not os.path.exists(f"{outdir}/temp"): 
            os.mkdir(f"{outdir}/temp")

        data, N = Reader.CsvFile(filename, all_cols=["c1","c2","c3","c4","c5","c6","c7","c8","c9","c10","c11","c12","c13","c14"])
        GenerationalSummary(data, filename)

        #OutLocalSearch(data, filename,plot_offset)
