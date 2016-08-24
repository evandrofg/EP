public class Producer extends Thread {
    private Monitor monitor;
    private int number;
    private int cont;

    public Producer(Monitor m, int number) {
        monitor = m;
        this.number = number;
        this.cont = 0;
    }

    public void run() {

        boolean continua = true;
        while (continua) {
            cont++;
            continua = monitor.enche_prato(number, cont);
            try {sleep((int)(1 * 100));
            } catch (InterruptedException e) {}
            
        }
    }
}
