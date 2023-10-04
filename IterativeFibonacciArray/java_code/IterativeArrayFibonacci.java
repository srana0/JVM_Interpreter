public class IterativeArrayFibonacci{
    public static void theThing(int d1, int d2, int d3, int d4,int value){
        int zero = 0;
        int one = 1;
        int two = 2;
        int[] a = new int[value+one];
        int result = zero;
        a[zero] = zero;
        if (value - one < 0) {
            result = a[value];
        }
        else {
            a[one] = one;
            for (int i = two ;  i-value - one < 0; i++){
                a[i] = a[i-one] + a[i-two];
            }
        }
        result =  a[value];
        System.out.println(result);
    }
    public static void main (String[] args){
        int value = Integer.parseInt(args[0]);
        theThing(0,0,0,0,value);
    }
}