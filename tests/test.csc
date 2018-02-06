import streams

function main()
    var list = {0, 1, 1, 2, 3, 3, 3, 5}.to_list()
    system.out.println(type(streams.of(list)))
    system.out.println(streams.of(list).skip(5).reverse().count())
end

main()

