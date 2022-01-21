matrix = [[8, 3, 1, 5],
          [11, 7, 1, 6],
          [7, 8, 6, 8],
          [11, 6, 4, 9]]

total2 = 0
total3 = 0
total4 = 0

res = []

for j_1 in range(4):
    total = 0
    total = matrix[0][j_1]
    for j_2 in range(4):
        if j_2 == j_1:
            continue
        else:
            total2 = total + matrix[1][j_2]

        for j_3 in range(4):
            if j_3 == j_1 or j_2 == j_3:
                continue
            else:
                total3 = total2 + matrix[2][j_3]

            for j_4 in range(4):
                if j_4 == j_1 or j_2 == j_4 or j_3 == j_4:
                    continue
                else:
                    total4 = total3 + matrix[3][j_4]
                    print(str(j_1) + " " + str(j_2) + " " + str(j_3) + " " + str(j_4) + " " + str(total4))
                    res.append(total4)

print("Min = " + str(min(res)))
