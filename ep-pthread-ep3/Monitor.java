public class Monitor {
    private int contents;
    private int Capacidade;
    private int Repeticoes;
    private int Contador;
    private boolean ControleProdutor;
    private boolean ControleConsumidor;
    
    private boolean full = false;
    private boolean empty = true;

    public Monitor(int C, int R){
        this.Capacidade = C;
        this.Repeticoes = R;
        this.Contador = 0;
        this.ControleProdutor = true;
        this.ControleConsumidor = true;
    }

    void signal() { super.notify(); }

    void signal_all() { super.notifyAll(); }

    public synchronized boolean get(int who, int cont) {
        if (ControleConsumidor == false) return false;
        while (empty == true) {
            try {
                wait();
            } catch (InterruptedException e) { }
        }
        // se nao temos mais porcoes, para
        if (ControleConsumidor == false) return false;
        contents--;
        System.out.println("Selvagem " + who + " pegou sua porção n° " + cont + ", restam " + contents  +" no prato.");
        if (contents == 0){
            System.out.println("Selvagem " + who + " notou o pote vazio.");
            empty = true;
            if (ControleProdutor == false) // ninguem vai produzir mais; sair
                ControleConsumidor = false;
        }
        full = false;
        
        signal_all();
        return ControleConsumidor;
    }

    public synchronized boolean enche_prato(int who, int cont) {
        if (ControleProdutor == false) return false;
        while (empty == false) {
            try {
                wait();
            } catch (InterruptedException e) { }
        }
        // se algum produtor ja produziu o ultimo, esse termina
        if (ControleProdutor == false)
            return false;        
        contents = Capacidade;
        Contador++;
        full = true;
        empty = false;
        System.out.println("Cozinheiro " + who + " encheu o prato pela " + cont + "ª vez.");
        signal_all();
        if (Contador == Repeticoes){
            ControleProdutor = false;
            //System.out.println("Parando após " + Contador + " repetições." );
        }
        return ControleProdutor;
    }
}
