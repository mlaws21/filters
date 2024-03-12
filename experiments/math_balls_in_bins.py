def place_balls_in_buckets(n, b, s, current_placement, placements):
    if n == 0:  # All balls are placed
        placements.append(list(current_placement))
        return

    if b == 0:  # No more buckets
        return

    for balls_in_bucket in range(min(n, s) + 1):  # Allow empty buckets
        current_placement.append(balls_in_bucket)
        place_balls_in_buckets(n - balls_in_bucket, b - 1, s, current_placement, placements)
        current_placement.pop()

n = 4  # Number of balls
b = 4  # Number of buckets
s = 2  # Maximum balls per bucket

placements = []
place_balls_in_buckets(n, b, s, [], placements)

padded = [x + ([0] * (b - len(x))) for x in placements]
print(padded)






# want an array that corrisponds to buckets
# [0, 2, 0, 1, 1, 3] -- valid permutation for s = 3; n = 7; l = 6