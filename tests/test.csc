import streams

function main()
    var list = {1, 2, 3, 4, 5, 6, 7, 8}.to_list()

    system.out.println(type(streams.of(list)))
    system.out.println(streams.of(list).skip(7).find_first())

    system.out.println("")
    streams.of(list).for_each([] (it) -> system.out.println(it))

    system.out.println("")
    streams.of(list).filter([] (it) -> it > 5).for_each([] (it) -> system.out.println(it))

    system.out.println("")
    streams.of(list).filter([] (it) -> it > 5).reverse().for_each([] (it) -> system.out.println(it))
end

main()

