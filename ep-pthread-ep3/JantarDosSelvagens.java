import java.util.*;
import java.io.*;

public class JantarDosSelvagens {
    public static void main(String[] args) {

        int N = 5; // numero de selvagens = consumidores
        int M = 1; // numero de cozinheiros = produtores
        int C = 20; // capacidade
        int R = 2;
        String pesos = " ";

        List<Producer> listP = new ArrayList<Producer>();
        List<Consumer> listC = new ArrayList<Consumer>();

        /* lê os valores da entrada */
        System.out.println("R: " + args[1] + " Tipo: " + args[2] );
        R = Integer.valueOf(args[1]);

        try {
            FileReader inputFile = new FileReader(args[0]);
            BufferedReader bufferReader = new BufferedReader(inputFile);
            N = Integer.valueOf( bufferReader.readLine() );
            pesos = bufferReader.readLine();
            C = Integer.valueOf( bufferReader.readLine() );
            M = Integer.valueOf( bufferReader.readLine() );
            bufferReader.close();
        }
        catch (Exception e){
            System.out.println("Erro ao ler arquivo de entrada:" + e .getMessage());
        }
        
        Monitor m = new Monitor(C, R);

        for (int i = 1; i <= M; i++)
            listP.add(new Producer (m, i));

        Scanner sc = new Scanner(pesos);
        for (int i = 1; i <= N; i++){
            if (args[2].equals("U"))
                listC.add(new Consumer (m, i, N, N));
            else
                listC.add(new Consumer (m, i, sc.nextInt(), N));
        }

        for (Producer produtor : listP)
            produtor.start();
        for (Consumer consumidor : listC)
            consumidor.start();
    }
}
