import streams

function greater_than_5(it)
    return it > 5
end

function main()
    var list = {1, 2, 3, 4, 5, 6, 7, 8}.to_list()

    system.out.println(type(streams.of(list)))
    system.out.println(streams.of(list).skip(7).find_first())

    system.out.println("")
    streams.of(list).for_each(system.out.println)

    system.out.println("")
    streams.of(list).limit(6).for_each(system.out.println)

    system.out.println("")
    streams.of(list).filter(greater_than_5).for_each(system.out.println)

    system.out.println("")
    streams.of(list).filter([] (it) -> it > 5).reverse().for_each(system.out.println)

    system.out.println("")
    streams.of(list).map([](it) -> it + 1).for_each(system.out.println)

    system.out.println("")
    system.out.println(streams.of(list).reduce(0, [](acc, it) -> acc + it))

    system.out.println("")
    system.out.println(streams.of(list).map([](it) -> it + 100).reduce(0, [](acc, it) -> acc + it))
end

main()

