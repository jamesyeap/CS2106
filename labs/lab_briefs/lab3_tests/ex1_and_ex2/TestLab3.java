import java.util.*;

public class TestLab3 {
	static int N = 2; // number of balls in each pack

	public static void main(String[] args) {
		Scanner sc = new Scanner(System.in);

		/* [TEST 1] Checks if packed balls correspond to each other */
		//	EXAMPLES:
		//	
		//	[CORRECT]
		//		Ball 335 was matched with ball 180
		//		Ball 180 was matched with ball 335
		//	
		//	[WRONG]
		//		Ball 335 was matched with ball 180
		//		Ball 180 was matched with ball 2
			
		HashMap<Integer,List<Integer>> h = new HashMap<>();

		String buffer;
		String[] onlyIntegers;
		int[] balls = new int[N];

		while (sc.hasNextLine()) {
			buffer = sc.nextLine();
			onlyIntegers = buffer.replaceAll("[^\\d.]", " ").replaceAll("\\s{2,}", " ").trim().split(" ");
			for (int i=0; i<N; i++) {
				balls[i] = Integer.parseInt(onlyIntegers[i]);
			}

			List<Integer> matchedWith = new ArrayList<>(N-1);
			for (int i=1; i<N; i++) {
				matchedWith.add(balls[i]);
			}
			h.put(balls[0], matchedWith);
		}

		System.out.format("\n****************************************[STARTING TEST 1]****************************************\n");
		Set<Map.Entry<Integer,List<Integer>>> s = h.entrySet();		
		for (Map.Entry<Integer,List<Integer>> ball : s) {
			int ballId = ball.getKey();
			List<Integer> matchedWith = ball.getValue();

			System.out.format("\nChecking %d, which is supposedly matched with %s\n", ballId, matchedWith.toString());
			for (Integer otherId : matchedWith) {
				if (!h.get(otherId).contains(ballId)) {
					System.out.format("%d has matched with %d, but %d matched with %s\n", ballId, otherId, otherId, h.get(otherId).toString());

					System.out.format("\n****************************************[TEST 1 - FAILED]****************************************\n\n");
					return;
				} else {
					// System.out.format("%d matched with %d\n", otherId, ballId);
					System.out.format("%d... OK\n", otherId);
				}
			}
		}

		System.out.format("\n****************************************[TEST 1 - OK]****************************************\n\n");
	}
}



