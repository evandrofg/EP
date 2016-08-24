public class Consumer extends Thread {
    private Monitor monitor;
    private int number;
    private int peso;
    private int cont;
    private int familia; // guarda quantos selvagens existem

    public Consumer(Monitor m, int number, int peso, int familia) {
        monitor = m;
        this.number = number;
        this.cont = 0;
        this.peso = peso;
        this.familia = familia;
    }

    public void run() {
        int value = 0;
        boolean continua = true;
        while (continua) {
            cont++;
            continua = monitor.get(number, cont);
            try {sleep((int)((familia - this.peso + 1) * 100));
            } catch (InterruptedException e) {}
        }
    }
}

