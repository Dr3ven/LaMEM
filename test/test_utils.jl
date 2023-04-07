# These are tools that help perform the LaMEM tests, which run LaMEM locally
using LinearAlgebra
"""
    run_lamem_local_test(ParamFile::String, cores::Int64=1, args::String=""; 
                        outfile="test.out", bin_dir="../../bin", opt=true, deb=false,
                        mpiexec="mpiexec")

This runs a LaMEM simulation with given `ParamFile` on 1 or more cores, while writing the output to a local log file.

"""
function run_lamem_local_test(ParamFile::String, cores::Int64=1, args::String=""; 
                outfile="test.out", bin_dir="../../bin", opt=true, deb=false,
                mpiexec="mpiexec")
    
    cur_dir = pwd()
    if opt
        exec=joinpath(cur_dir,bin_dir,"opt","LaMEM")
    elseif deb
        exec=joinpath(cur_dir,bin_dir,"deb","LaMEM")
    end

    success = true
    try
        if cores==1
            perform_run = `$(exec) -ParamFile $(ParamFile) $args`;
            
            # Run LaMEM on a single core, which does not require a working MPI
            try 
                if !isempty(outfile)
                    run(pipeline(perform_run, stdout=outfile));
                else
                    run(perform_run);
                end
            catch
                println("An error occured in directory: $(cur_dir) ")
                println("while running the script:")
                println(perform_run)
                success = false
            end
        else
            perform_run = `$(mpiexec) -n $(cores) $(exec) -ParamFile $(ParamFile) $args`;
            # set correct environment
            #mpirun = setenv(mpiexec, LaMEM_jll.JLLWrappers.JLLWrappers.LIBPATH_env=>LaMEM_jll.LIBPATH[]);
            # Run LaMEM in parallel
            try 
                if !isempty(outfile)
                    run(pipeline(perform_run, stdout=outfile));
                else
                    run(perform_run);
                end
            catch
                println(perform_run)
                success = false
            end
        end
    catch
        success = false
    end
  
    return success
end


"""
    out = extract_info_logfiles(file::String, keywords::NTuple{N,String}=("|Div|_inf","|Div|_2","|mRes|_2"), split_sign="=", remove_substrings="")

Extracts values from the logfile `file` specified after `keywords` (optionally defining a `split_sign`).
This will generally return a NamedTuple with Vectors 

"""
function extract_info_logfiles(file::String, keywords::NTuple{N,String}=("|Div|_inf","|Div|_2","|mRes|_2"), split_signs="=") where N

    split_sign = split_signs;
    out=()
    for i=1:N        
        d =  []
        if isa(split_signs,Tuple)
            split_sign = split_signs[i]
        end
        
        open(file) do f
            while ! eof(f) 
                # read a new / next line for every iteration          
                line = readline(f)
                if contains(line, keywords[i])
                    num = extract_value_from_string(line, keywords[i], split_sign);
                    push!(d,num)    # add value to vector
                end
            end
        end
        
        out =  (out..., Float64.(d))    # add vector to tuple
    end

    out_NT = NamedTuple{Symbol.(keywords)}(out)

    return out_NT
end


"""
    extract_value_from_string(line_origin::String, keyword::String, split_sign="=", type=Float64)

This extracts a numerical value from `line_origin` after the `keyword`. Optionally, there can be a `split_sign` in between and we can transfer it to another `type` than `Float64`.`
"""
function extract_value_from_string(line_origin::String, keyword::String, split_sign="=", type=Float64)
    
    if length(split_sign)==0
        num = extract_lastvalue_from_string(line_origin, keyword, type)
    else
        # find keyword
        ind  = findlast(keyword,line_origin)
        line = line_origin[ind[end]+1:end]

        # remove split signs
        line = replace(line, split_sign=>"") 
        line = replace(line, ","=>"")           # comma

        # Extract value
        num=NaN
        if !isempty(ind)
            line_vec = split(line)
            try
                num = parse(type,line_vec[1])
            catch
                println("Problem parsing line: $line_origin")
            end
        end
    end

    return num
end

"""
    extract_lastvalue_from_string(line_origin::String, keyword::String, type=Float64)

This extracts the last numerical value from `line_origin` after the `keyword`.
"""
function extract_lastvalue_from_string(line_origin::String, keyword::String, type=Float64)
    
    # find keyword
    ind  = findlast(keyword,line_origin)
    line = line_origin[ind[end]+1:end]

    # Extract value
    num=[];
    line_vec = split(line)
    for i=1:lastindex(line_vec)
        val=nothing
        try
            val = parse(type,line_vec[i])
        catch
        end
        if !isnothing(val)
            push!(num,val)
        end
    end
    
    return num[1]
end


"""
    success = compare_logfiles(new::String, expected::String, 
                        keywords::NTuple{N,String}=("|Div|_inf","|Div|_2","|mRes|_2"), 
                        accuracy=((rtol=1e-6,), (rtol=1e-6,), (rtol=1e-6,)),
                        split_sign="=",
                        remove_substring="")

This compares two logfiles (different parameters which can be indicated). If the length of the vectors is not the same, or the accuracy criteria are not met, `success=false` and info is displayed, to help track down the matter.
We scan the file for lines with the given `keywords`, and extract numerical values from it.

Arguments:
    `split_sign`        : Can be a `Tuple`, containing the sign after which we split the string
    
"""
function compare_logfiles(new::String, expected::String, 
                        keywords::NTuple{N,String}=("|Div|_inf","|Div|_2","|mRes|_2"), 
                        accuracy=((rtol=1e-6,), (rtol=1e-6,), (rtol=1e-6,));
                        split_sign="=") where N

    new_out = extract_info_logfiles(new, keywords, split_sign)
    exp_out = extract_info_logfiles(expected, keywords, split_sign)
    
    test_status = true
    for i=1:N 
        rtol, atol = 0,0
        if  haskey(accuracy[i], :rtol)
            rtol = accuracy[i].rtol;
        end
        if  haskey(accuracy[i], :atol)
            atol = accuracy[i].atol;
        end
        if length(new_out[i])==length(exp_out[i])
            te =  isapprox(new_out[i], exp_out[i], rtol=rtol, atol=atol)
            if te==false
                println("Problem with comparison of $(keywords[i]):")
                print_differences( new_out[i], exp_out[i], accuracy[i])
                test_status = false
            end
        else
            println("Problem with comparison of $(keywords[i]):")
            println("length of vectors not the same (new: $(length(new_out[i])), expected: $(length(exp_out[i]))")
            test_status = false
        end
       
    end
  

    return test_status
end

# Pretty formatting of errors
function print_differences(new, expected, accuracy)
    n = 24;
    atol_expected = 0.0;
    if haskey(accuracy, :atol)
        atol_expected = accuracy.atol;
    end
    rtol_expected = 0.0;
    if haskey(accuracy, :rtol)
        rtol_expected = accuracy.rtol;
    end

    println("      $(rpad("New",n)) | $(rpad("Expected",n)) | $(rpad("rtol (<$(rtol_expected))",n)) | $(rpad("atol  (<$(atol_expected))",n))")

    for i in eachindex(new)
        atol = norm(new[i] - expected[i])
        rtol = atol/max(norm(new[i]), norm(expected[i]))
        col = :normal
        if atol >  max(atol_expected, rtol_expected*max(norm(new[i]), norm(expected[i])))
            col = :red
        end
        printstyled("$(rpad(i,4))  $(rpad(new[i],n)) | $(rpad(expected[i],n)) | $(rpad(rtol,n)) | $(rpad(atol,n)) \n", color=col)
    end

    return nothing
end


"""
    perform_lamem_test( dir::String, 
                        ParamFile::String, 
                        expectedFile::String; 
                        keywords=("|Div|_inf","|Div|_2","|mRes|_2"), 
                        accuracy=((rtol=1e-6,atol=1e-10), (rtol=1e-6,), (rtol=1e-6,)), 
                        cores::Int64=1, 
                        args::String="",
                        bin_dir="../bin",  
                        opt=true, 
                        deb=false, 
                        mpiexec="mpiexec",
                        split_sign="=", 
                        debug::Bool=false, 
                        create_expected_file::Bool=false)

This performs a LaMEM simulation and compares certain keywords of the logfile with results of a previous simulation        

Parameters:
- `dir`: directory in which the LaMEM `*.dat` ParamFile is located
- `ParamFile`: name of the LaMEM input file
- `expectedFile`: name of the file with earlier results, versus which we compare
- `keywords`: Tuple with keywords, which contain numerical values
- `accuracy`: Tuple that contains relative (`rtol`), and (optionally) absolute `atol` tolerances
- `cores`: Number of cores on which to perform the test
- `args`: Optional LaMEM command line Arguments
- `bin_dir`: directory where the LaMEM binaries are, relative to the current one
- `opt`: run with optimized LaMEM?
- `deb`: run with debug version of LaMEM?
- `mpiexec`: mpi executable
- `split_sign`: split sign (or Tuple of it)
- `debug`: set to true if you simply want to see the output of the simulation (no test done)
- `create_expected_file`: create an expected file

"""
function perform_lamem_test(dir::String, ParamFile::String, expectedFile::String; 
                keywords=("|Div|_inf","|Div|_2","|mRes|_2"), accuracy=((rtol=1e-6,), (rtol=1e-6,), (rtol=1e-6,)), 
                cores::Int64=1, args::String="",
                bin_dir="../bin",  opt=true, deb=false, mpiexec="mpiexec",
                split_sign="=", 
                debug::Bool=false, create_expected_file::Bool=false,
                )

    cur_dir = pwd();
    cd(dir)

    bin_dir = joinpath(cur_dir,bin_dir);
    if debug==true
        outfile = "";
    else
        outfile = "test_$(cores).out";
    end
    if create_expected_file==true
        outfile = expectedFile;
        debug = true;
    end

    # perform simulation 
    success = run_lamem_local_test(ParamFile, cores, args, outfile=outfile, bin_dir=bin_dir, opt=opt, deb=deb, mpiexec=mpiexec);

    if success==true && debug==false
        # compare logfiles 
        success = compare_logfiles(outfile, expectedFile, keywords, accuracy, split_sign=split_sign)
    end
    if create_expected_file==true
        println("Created expected file: $expectedFile in directory $dir")
    end
    if !success
        # something went wrong with executing the file (likely @ PETSc error)
        # Display some useful info here that helps debugging
        println("Problem detected with test; see this on commandline with: ")
        println("  dir=$(joinpath(cur_dir,dir)) ")
        println("  ParamFile=$(ParamFile) ")
        println("  cores=$(cores) ")
        println("  args=$(args) ")
        println("  outfile=$(outfile) ")
        println("  bindir=$(bin_dir) ")
        println("  opt=$(opt) ")
        println("  deb=$(deb) ")
        println("  mpiexec=$(mpiexec) ")
        println("  success = run_lamem_local_test(ParamFile, cores, args, outfile=nothing, bin_dir=bin_dir, opt=opt, deb=deb, mpiexec=mpiexec);")
    end

    cd(cur_dir)  # return to directory       
    
    return success
end 

