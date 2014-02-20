/* Tests for variable renaming.  Any variable whose name begins with "tmp" is renamed so it doesn't conflict with
 * other variables that might be visible at the insertion point. */
public class Snippets6 {

    void randomOffByOne(int arg1) {
        java.util.Random tmp_random = new java.util.Random();
        int tmp_offset = tmp_random.nextInt() % 3 - 1;
        arg1 += tmp_offset;
    }

    public void notNeeded() {
        // DO_NOT_INSERT
        System.out.println("this function should not be inserted into the target");
    }

    void addWithError(int addend1, int addend2, int result) {
        randomOffByOne(addend1);
        randomOffByOne(addend2);
        result = addend1 + addend2;
    }
}
