import numpy as np
import matplotlib.pyplot as plt

x_number = np.array([0,1,2,3,4])
x_label = ['10','50','100','200','500']
abp = [0.009019,  0.009019 ,   0.009019,    0.009019,    0.009019]
gbn = [0.019, 0.019, 0.019, 0.019,    0.019]
sr = [0.019381,  0.019381, 0.019381,    0.019381,    0.019381]

plt.xticks(x_number,x_label)
plt.plot(abp,'-o',color='orange')
plt.plot(gbn,'-o',color='green')
plt.plot(sr,'-o',color='blue')

plt.xlabel('Window Size',fontsize=10)
plt.ylabel('Throughput',fontsize=10)
plt.legend(('abp', 'gbn','sr'))
plt.title('Experiment 2 Loss Probability 0.4')
plt.savefig('exp2_loss04',dpi=300)

plt.show()