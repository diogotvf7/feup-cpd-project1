function OnMult(m_ar::Int, m_br::Int)
    pha = ones(Float64, m_ar, m_ar)
    phb = [fill(i, m_br) for i in 1:m_br]
    phc = zeros(Float64, m_ar, m_br)

    elapsed_time = @elapsed begin
        for i in 1:m_ar
            for j in 1:m_br
                temp = 0.0
                for k in 1:m_ar
                    temp += pha[i, k] * phb[k][j]
                end
                phc[i, j] = temp
            end
        end
    end

    println("Elapsed time: ", elapsed_time, " seconds")
    printMx(m_br, phc)

    return elapsed_time
end

function OnMultLine(m_ar::Int, m_br::Int)
    pha = ones(Float64, m_ar, m_ar)
    phb = [fill(i, m_br) for i in 1:m_br]
    phc = zeros(Float64, m_ar, m_br)

    elapsed_time = @elapsed begin
        for k in 1:m_ar
            for j in 1:m_br
                for i in 1:m_ar
                    phc[i, j] += pha[i, k] * phb[k][j]
                end
            end
        end
    end

    println("Elapsed time: ", elapsed_time, " seconds")
    printMx(m_br, phc)

    return elapsed_time
end

function OnMultBlock(m_ar::Int, m_br::Int, bkSize::Int)
    pha = ones(Float64, m_ar, m_ar)
    phb = [fill(i, m_br) for i in 1:m_br]
    phc = zeros(Float64, m_ar, m_br)

    elapsed_time = @elapsed begin
        for ii in 1:bkSize:m_ar
            for kk in 1:bkSize:m_br
                for jj in 1:bkSize:m_br
                    for i in ii:min(ii+bkSize-1, m_ar)
                        for k in kk:min(kk+bkSize-1, m_ar)
                            for j in jj:min(jj+bkSize-1, m_br)
                                phc[i, j] += pha[i, k] * phb[k][j]
                            end
                        end
                    end
                end
            end
        end
    end
        
    println("Elapsed time: ", elapsed_time, " seconds")
    printMx(m_br, phc)

    return elapsed_time
    
end

function printMx(matrix_size::Int, matrix::Array{Float64, 2})
    println("Result matrix:")
    for i in 1:min(10, matrix_size)
        print(matrix[1, i], " ")
    end
    println()
end

using PAPI

function main()
    events = Event[PAPI.L1_DCM, PAPI.L2_DCM, PAPI.TOT_INS, PAPI.TOT_CYC]
    values = Vector{Counts}(undef, length(events))
    
    evtset = start_counters(events)
    
    op::Int = 1
    while op != 0
        println("\n1. Multiplication")
        println("2. Line Multiplication")
        println("3. Block Multiplication")
        println("Selection: ")
        op = parse(Int, readline())

        println("Matrix Dimensions: ")
        n = parse(Int, readline())

        if op == 1
            elapsed_time = OnMult(n, n)
        elseif op == 2
            elapsed_time = OnMultLine(n, n)
        elseif op == 3
            println("Block Size: ")
            blockSize = parse(Int, readline())
            elapsed_time = OnMultBlock(n, n, blockSize)
        else
            println("Invalid choice")
        end
        read_counters!(evtset, values)

        println("Elapsed time: ", elapsed_time, " seconds")

        # Matrix Size, Time, DCM1, DCM2, TOT_INS, TOT_CYC
        results = [n, elapsed_time]
        append!(results, values)
        println("Results:")
        println(join(results, ", "))

    end

end

main()
